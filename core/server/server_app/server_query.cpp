/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: query message and return for client
 */

#include <cstdio>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <stdio.h> 
#include <chrono>
#include <thread>
#include "timer.h"

#include "server_query.h"
#include "server_disk_manager.h"
#include "server_database_calls.h"

#include "config_macros.h"      
#include "utils_sgx.h"
#include "utils.h"
#include "server_enclave_u.h"
#include HTTPLIB_PATH

#include "sgx_urts.h"
#include "sgx_eid.h"
#include "sgx_ukey_exchange.h"
#include "sgx_uae_epid.h"
#include "sgx_uae_quote_ex.h"
#include "sgx_tcrypto.h"

using namespace httplib;

server_error_t parse_query(char* msg, access_message_t* p_rcv_msg)
{
    int ret = 0;

    if(DEBUG_TIMER) Timer t("parse_query");
    if(DEBUG_PRINT) printf("\nParsing query message fields\n");

    char* token = strtok_r(msg, "|", &msg);
    int i = 0;
    char auxiliar[3];
    char* invalid_char;
    while (token != NULL)
    {
        i++;
        token = strtok_r(NULL, "|", &msg);

        // Get client key
        if (i == 1){
            memcpy(p_rcv_msg->pk, token, 8);
            p_rcv_msg->pk[8] = '\0';

            if(DEBUG_PRINT) printf("pk: %s\n", p_rcv_msg->pk);
        }

        // Get data index
        if (i == 3) {

            p_rcv_msg->index = (uint32_t)strtoul(token, &invalid_char, 10);

            if(*invalid_char != 0) 
                return print_error_message(INVALID_INDEX_FIELD_ERROR);
           
            if(DEBUG_PRINT) printf("index: %u\n", p_rcv_msg->index); 
        }
        // Get command size
        if (i == 5) {
            p_rcv_msg->command_size = (uint32_t)strtoul(token, &invalid_char, 16);

            if(*invalid_char != 0) {
                printf("\nInvalid command size message format.\n");
                return INVALID_COMMAND_SIZE_FIELD_ERROR;
            }

            if(DEBUG_PRINT) printf("command_size: %u\n", p_rcv_msg->command_size);
        }

        // Get command 
        if (i == 7) { 

            p_rcv_msg->command = (char*)malloc(1+p_rcv_msg->command_size);
            memcpy(p_rcv_msg->command, token, p_rcv_msg->command_size);
            p_rcv_msg->command[p_rcv_msg->command_size] = 0;

            if(DEBUG_PRINT) printf("command: %s\n", p_rcv_msg->command);
        }
        
        // Get encrypted 
        if (i == 9) {
            ret = convert_text_to_buffer(token, 3*(8+16+12), p_rcv_msg->encrypted, NULL);
            p_rcv_msg->encrypted[8+12+16] = '\0';
        }
    }

    if(ret) 
        return print_error_message(INVALID_ENCRYPTED_FIELD_ERROR);

    return OK;
}

server_error_t get_query_message(const Request& req, char* snd_msg, uint32_t* p_size)
{
    if(DEBUG_TIMER) Timer t("get_query_message");
    if(DEBUG_PRINT) printf("\nGetting query message fields:\n");

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

    if(DEBUG_PRINT) printf("Message: %s\n\n", snd_msg);

    return OK;
}

server_error_t get_response(stored_data_t stored, 
                         sgx_enclave_id_t global_eid, 
                         uint8_t* response, 
                         access_message_t rcv_msg)
{
    if(DEBUG_TIMER) Timer t("enclave_get_response");

    server_error_t ret;

    uint8_t* querier_sealed_data = (uint8_t*)malloc(SEALED_SIZE);
    uint32_t querier_seald_size;
    if(ret = read_user_key_file(rcv_msg.pk, querier_sealed_data, &querier_seald_size)){
        free(querier_sealed_data);
        return ret;
    }

    uint8_t* storage_sealed_data = (uint8_t*)malloc(SEALED_SIZE);
    uint32_t storage_sealed_size;
    if(ret = read_storage_key_file(rcv_msg.pk, storage_sealed_data, &storage_sealed_size)){
        free(querier_sealed_data);
        free(storage_sealed_data);
        return ret;
    }
    //printf("%s\n", querier_pk);

    // Call enclave to unseal keys, decrypt with the querier key and encrypt with querier key
    {
    if(DEBUG_TIMER) Timer t2("retrieve_data");
    if(DEBUG_PRINT) printf("\nEntering enclave to verify access permissions\n");

    sgx_status_t sgx_ret;
    sgx_status_t ecall_status;
    int enc_ret = (int)OK;
    sgx_ret = enclave_retrieve_data(global_eid, &ecall_status,
        DEBUG_PRINT,
        (sgx_sealed_data_t*)querier_sealed_data,
        (sgx_sealed_data_t*)storage_sealed_data,
        rcv_msg.encrypted,
        stored.encrypted,
        stored.encrypted_size,
        rcv_msg.pk,
        response,
        &enc_ret);
    if(DEBUG_PRINT) printf("Exiting enclave\n");

    ret = (server_error_t) enc_ret;

    free(querier_sealed_data);
    free(storage_sealed_data);

    if(sgx_ret != SGX_SUCCESS || ecall_status != SGX_SUCCESS || ret != OK) {
        printf("SGX return error code: 0x%04x\n Ecall status error code: 0x%04x\n", (int)sgx_ret, (int)ecall_status);
        if(ret != OK) return print_error_message(ret);
        else return print_error_message(RETRIEVE_DATA_ENCLAVE_ERROR);
    }
    }
    return OK;
}

void make_response(uint8_t* enc_data, uint32_t enc_data_size, char* response)
{
    if(DEBUG_TIMER) Timer t("make_response");
    sprintf(response, "size|0x%02x|data|", enc_data_size);
    char auxiliar[7];
    for (uint32_t count=0; count<enc_data_size; count++)
    {
        sprintf(auxiliar, "%02x-", enc_data[count]);
        memcpy(&response[15+count*3], auxiliar, 3);
    }
    response[15+enc_data_size*3] = '\0';
    
    if(DEBUG_PRINT) printf("Sending message: %s\n", response);
}

server_error_t server_query(const Request& req, Response& res, sgx_enclave_id_t global_eid)
{
    if(DEBUG_TIMER) Timer t("server_query");
    server_error_t ret = OK;

    // Get message sent in HTTP header
    char* snd_msg = (char*)malloc(URL_MAX_SIZE);

    uint32_t size;
    ret = get_query_message(req, snd_msg, &size);
    if(ret) {
        free(snd_msg);
        return ret;
    }

    // Get data index and pk
    access_message_t rcv_msg;
    ret = parse_query(snd_msg, &rcv_msg);
    free(snd_msg);
    if(ret) return ret;

    // Query data from db
    char* data = (char*)malloc(MAX_DATA_SIZE);
    uint32_t data_size;
    if(ret = query_db(rcv_msg.command, rcv_msg.index, data, &data_size)){
       free(data);
       return ret; 
    }

    // Separate parameters of stored data
    stored_data_t message; 
    ret = get_stored_parameters(data, &message);
    free(data);
    if(ret) return ret;

    // Verify access permissions
    uint8_t *enc_data = (uint8_t*)malloc(message.encrypted_size);
    char *response = (char*)malloc(15+3*message.encrypted_size+1);

    if(ret = get_response(message, global_eid, enc_data, rcv_msg)) {
        if(DEBUG_PRINT) printf("\nAccess accepted\n");
        make_response(enc_data, message.encrypted_size, response);
        res.set_content(response, "text/plain");
    }
    free(response);
    free(enc_data);
    free(message.encrypted);

    return ret; 
}