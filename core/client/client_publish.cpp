/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Description: send data for publishing
 */

#include <cstdio>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <chrono>
#include <thread>
//#include "timer.h"
#include "errors.h"

#include "client_publish.h"

//#include "sample_libcrypto.h"   
#include "config_macros.h"        
#include HTTPLIB_PATH
#include "encryption.h"
#include "utils.h"

int send_data_for_publication(char* time, char* pk, char* type, uint8_t* enc_data, uint32_t enc_data_size)
{
    // Build publication message
    // "http://localhost:7778/publish/size=631/time|10h20m33s|pk|72d41281|type|555555|size|62|encrypted|dd-b1-b6-b8-22-d3-9a-..."
    size_t header_size = 5+20+3+9+5+7+5+3+10;
    size_t snd_msg_size = (header_size+1+3*enc_data_size)*sizeof(char);

    char* snd_msg = (char*)malloc(snd_msg_size);
    memset(snd_msg, 0, snd_msg_size);
    sprintf(snd_msg, "time|%s|pk|%s|type|%s|size|%02x|encrypted|", time, pk, type, (unsigned int)enc_data_size);

    convert_buffer_to_text(enc_data, enc_data_size, &snd_msg[header_size], NULL);

    // Build HTTP publication message
    char* http_message = (char*)malloc(URL_MAX_SIZE);
    sprintf(http_message, "/publish/size=%d/%s", (int)snd_msg_size, snd_msg);
    free(snd_msg);

    // Send HTTP publication message
    httplib::Error err = httplib::Error::Success;
    httplib::Client cli(SERVER_URL, SERVER_PORT);
    std::this_thread::sleep_for(std::chrono::milliseconds(LATENCY_MS));

    printf("Message %s\n", http_message);
    auto res = cli.Get(http_message);
    free(http_message);
    
    if (res) {

        if (res->status != 200) {
            printf("Error code: %d\n", (int)res->status);
            return (int)print_error_message(HTTP_RESPONSE_ERROR);
        }

        char* http_response = (char*)malloc(URL_MAX_SIZE);
        sprintf(http_response,"%s",res->body.c_str());
        if(!strcmp(http_response, "0")) {
            printf("Received ack\n");
            free(http_response);
            return 0;
        }

        char* invalid_char;
        server_error_t error = (server_error_t)strtoul(http_response, &invalid_char, 10);
        if(*invalid_char != 0) {
            free(http_response);
            return (int)print_error_message(INVALID_ERROR_CODE_FORMAT_ERROR);
        }
        free(http_response);
        return (int)print_error_message(error);
    } 
    
    else {
        err = res.error();
        printf("Error %d\n", (int)err);
        return (int)print_error_message(HTTP_SEND_ERROR);
    }

    return 0;
}

int client_publish(uint8_t* key, client_data_t data)
{
    // Mount text with client data
    // time|10h30m47s|pk|72d41281|type|123456|payload|250|permission1|72d41281
    uint32_t formatted_data_size = 5+20+3+9+5+7+8+(uint32_t)strlen(data.payload)+(13+8)*(data.permissions_count);
    char* formatted_data = (char*)malloc(sizeof(char) * (formatted_data_size+1)); // includes \n
    memset(formatted_data, 0, formatted_data_size+1);

    sprintf(formatted_data,"time|%s|pk|%s|type|%s|payload|%s", 
            data.time, data.pk, data.type, data.payload);

    char* permission = (char*)malloc(13+8+1);
    for(uint32_t index=0; index<data.permissions_count; index++) {
        sprintf(permission, "|permission%d|%s", index, data.permissions_list[index]);
        strncpy(formatted_data+strlen(formatted_data), permission, strlen(permission));
    }
    free(permission);
    printf("Data: %s\n", formatted_data);

    uint32_t enc_data_size = MAX_ENC_DATA_SIZE;
    uint8_t* enc_data = (uint8_t *) malloc(enc_data_size*sizeof(uint8_t));

    int ret = encrypt_data(key, enc_data, &enc_data_size, (uint8_t*)formatted_data, formatted_data_size); // does not includes \n
    free(formatted_data);
    if(ret != 0) {
        free(enc_data);
        return (int)print_error_message(CLIENT_ENCRYPTION_ERROR);
    }

    // Send data for publication
    int send_ret = send_data_for_publication(data.time, data.pk, data.type, enc_data, enc_data_size);
    free(enc_data);
    
    return send_ret;
}
