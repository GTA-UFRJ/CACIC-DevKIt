/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: process in enclave client data before publishing
 */

#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <string>
#include <stdexcept>
#include <limits>
#include <stdio.h>
#include <chrono>
#include <thread>
#include "timer.h"

#include "errors.h"
#include "server_publish.h"
#include "server_disk_manager.h"
#include "server_database_calls.h"

#include "config_macros.h"      // ULTRALIGH_SAMPLE
#include "utils_sgx.h"
#include "utils.h"
#include "server_enclave_u.h"
//#include "ecp.h"                // sample_ec_key_128bit_t
#include HTTPLIB_PATH

#include "sgx_urts.h"
#include "sgx_eid.h"
#include "sgx_ukey_exchange.h"
#include "sgx_uae_epid.h"
#include "sgx_uae_quote_ex.h"
#include "sgx_tcrypto.h"

using namespace httplib;


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "errors.h"

// Server ocall functions temporarily copied to here to avoid errors
// --------------------------------------------------------

void ocall_print_secret(uint8_t* secret, uint32_t secret_size)
{
    uint32_t i;
    char hex_number[5];
    for (i=0;i<secret_size;i++)
    {
        sprintf(hex_number, "%x", secret[i]);
        printf("%s ", hex_number);
    }
    printf("\n");
}
 
void ocall_print_number(long number) {
    printf("%ld\n", number);
}

void ocall_print_string(const char* text) {
    printf("%s\n", text);
}

void ocall_query_db(int* p_ret, char* command, uint32_t index, char* data, uint32_t* p_data_size) {
    *p_ret = (int) query_db(command, index, data, p_data_size);
}

void ocall_multi_query_db(
    int* p_ret, 
    char* command, 
    uint32_t command_size, 
    char** datas, 
    uint32_t* datas_sizes, 
    uint32_t* p_data_count) 
{
    *p_ret = (int) multi_query_db(command, datas, datas_sizes, p_data_count);
}

// --------------------------------------------------------




// time|12h30m57s|pk|72d41281|type|123456|size|62|encrypted|...
server_error_t parse_request(char* msg, iot_message_t* p_rcv_msg)
{
    int ret = 0;

    if(DEBUG_TIMER) Timer t("parse_request");   
    if(DEBUG_PRINT) printf("\nParsing publication message fields\n");
    
    char* token = strtok_r(msg, "|", &msg);
    int i = 0;
    char* invalid_char;

    while (token != NULL)
    {
        i++;
        token = strtok_r(NULL, "|", &msg);

        // Get client time
        if (i == 1){
            memcpy(p_rcv_msg->time, token, 19);
            p_rcv_msg->time[19] = '\0';

            if(DEBUG_PRINT) printf("time: %s\n", p_rcv_msg->time);
        }

        // Get client key
        if (i == 3){
            memcpy(p_rcv_msg->pk, token, 8);
            p_rcv_msg->pk[8] = '\0';

            if(DEBUG_PRINT) printf("pk: %s\n", p_rcv_msg->pk);
        }

        // Get data type
        if (i == 5) {
            memcpy(p_rcv_msg->type, token, 6);
            p_rcv_msg->type[6] = '\0';

            if(DEBUG_PRINT) printf("type: %s\n", p_rcv_msg->type);
        }

        // Get encrypted message size
        if (i == 7) {
            p_rcv_msg->encrypted_size = (uint32_t)strtoul(token, &invalid_char, 16);

            if(*invalid_char != 0)
                return print_error_message(INVALID_ENCRYPTED_SIZE_FIELD_ERROR);

            if(DEBUG_PRINT) printf("encrypted_size: %u\n", p_rcv_msg->encrypted_size); // does not includes \n
        }

        // Get encrypted message
        if (i == 9) {
            p_rcv_msg->encrypted = (uint8_t*)malloc((p_rcv_msg->encrypted_size+1) * sizeof(uint8_t));

            ret = convert_text_to_buffer(token, 3*(p_rcv_msg->encrypted_size), p_rcv_msg->encrypted, NULL);
            p_rcv_msg->encrypted[p_rcv_msg->encrypted_size] = '\0'; 
        }
    }

    if(ret) {
        free(p_rcv_msg->encrypted);
        return print_error_message(INVALID_ENCRYPTED_FIELD_ERROR);
    }

    return OK;
}

server_error_t get_publish_message(const Request& req, char* snd_msg, uint32_t* p_size)
{
    if(DEBUG_TIMER) Timer t("get_publish_message");
    if(DEBUG_PRINT) printf("\nGetting publish message fields:\n");

    std::string size_field = req.matches[1].str();

    try {
        *p_size = (uint32_t)std::stoul(size_field);
    }
    catch (std::invalid_argument& exception) {
        return print_error_message(INVALID_HTTP_MESSAGE_SIZE_FIELD_ERROR);
    }

    if(*p_size > URL_MAX_SIZE)
        return print_error_message(HTTP_MESSAGE_SIZE_OVERFLOW_ERROR);

    if(DEBUG_PRINT) printf("Size: %u\n", *p_size);

    std::string message_field = req.matches[2].str();

    strncpy(snd_msg, message_field.c_str(), (size_t)(*p_size-1));
    snd_msg[*p_size] = '\0';
    
    if(DEBUG_PRINT) printf("Message: %s\n", snd_msg);

    return OK;
}

server_error_t server_publish(const Request& req, Response& res, sgx_enclave_id_t global_eid)
{
    if(DEBUG_TIMER) Timer t("server_publish");
    server_error_t ret = OK;

    // Get message sent in HTTP header
    char* snd_msg = (char*)malloc(URL_MAX_SIZE);

    uint32_t size;
    ret = get_publish_message(req, snd_msg, &size);
    if(ret) {
        free(snd_msg);
        return ret;
    }
 
    // Server receives and separate parameters according to Ultrlight protocol
    // time|2012-05-06.21:47:59|pk|72d41281|type|123456|size|62|encrypted|... 
    iot_message_t rcv_msg;
    ret = parse_request(snd_msg, &rcv_msg);
    free(snd_msg);
    if(ret)
        return ret;

    uint32_t querier_seald_size;
    uint8_t* querier_sealed_data = (uint8_t*)malloc(SEALED_SIZE);
    if(ret = read_user_key_file(rcv_msg.pk, querier_sealed_data, &querier_seald_size)){
        free(querier_sealed_data);
        return ret;
    }

    uint32_t storage_sealed_size;
    uint8_t* storage_sealed_data = (uint8_t*)malloc(SEALED_SIZE);
    if(ret = read_storage_key_file(rcv_msg.pk, storage_sealed_data, &storage_sealed_size)){
        free(querier_sealed_data);
        free(storage_sealed_data);
        return ret;
    }

    uint32_t processed_data_size;
    char* processed_data = (char*)malloc(processed_data_size);

    sgx_status_t sgx_ret;
    sgx_status_t ecall_status;
    int enc_ret = (int)OK;
    sgx_ret = enclave_publication_wrapper(global_eid, &ecall_status,
        DEBUG_PRINT,
        (sgx_sealed_data_t*)querier_sealed_data,
        (sgx_sealed_data_t*)storage_sealed_data,
        rcv_msg.time,
        rcv_msg.pk,
        rcv_msg.type,
        rcv_msg.encrypted,
        rcv_msg.encrypted_size,
        processed_data,
        &processed_data_size,
        &enc_ret);
    if(DEBUG_PRINT) printf("\n------------ EXITED ENCLAVE -----------\n");
    

    ret = (server_error_t) enc_ret;

    free(querier_sealed_data);
    free(storage_sealed_data);
    free(rcv_msg.encrypted);

    if(sgx_ret != SGX_SUCCESS || ecall_status != SGX_SUCCESS || ret != OK) {
        free(processed_data);
        printf("\nSGX return error code: 0x%04x\nEcall status error code: 0x%04x\n", (int)sgx_ret, (int)ecall_status);
        if(ret != OK) return print_error_message(ret);
        else return print_error_message(PUBLICATION_ENCLAVE_ERROR);
    }
    
    if(ret = publish_db(rcv_msg.time, rcv_msg.pk, rcv_msg.type, processed_data, processed_data_size)){
        free(processed_data);
        return print_error_message(ret); 
    }

    // Send response
    if(DEBUG_PRINT) printf("Sending ack\n");
    res.set_content("0", "text/plain");

    return ret; 
}
