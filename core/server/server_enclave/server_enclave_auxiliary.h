/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: utilities for using inside enclave
 */

#ifndef _SERVER_ENCLAVE_AUXILIARY_
#define _SERVER_ENCLAVE_AUXILIARY_

#include "errors.h"
#include "server.h"
#include "sgx_trts.h"

sgx_status_t enclave_encrypt_data(    
    uint8_t* key,
    uint8_t* enc_data,
    uint8_t* plain_data,
    uint32_t plain_data_size);

sgx_status_t enclave_decrypt_data(    
    uint8_t* key,
    uint8_t* enc_data,
    uint32_t enc_data_size,
    uint8_t* plain_data);

server_error_t enclave_get_payload(
    uint8_t* decrypted, 
    uint32_t decrypted_size, 
    char* payload, 
    uint32_t* payload_size);

server_error_t enclave_verify_permissions(
    uint8_t* decrypted, 
    uint32_t decrypted_len, 
    char* pk, 
    bool* accepted);

server_error_t enclave_get_encrypted(
    char* data, 
    uint32_t data_size, 
    uint8_t* encrypted, 
    uint32_t* p_encrypted_size);

server_error_t enclave_get_permissions(
    char* plain_data,
    uint32_t plain_data_size,
    char* permissions,
    uint32_t* p_permissions_size);
    
void enclave_free_data_array(
    char** datas, 
    uint32_t* datas_sizes, 
    uint32_t data_count);

server_error_t enclave_query_db(
    uint8_t* key,
    char* command,
    uint32_t index,
    char* data,
    uint32_t* p_data_size);

server_error_t enclave_multi_query_db(
    char* pk,
    uint8_t* key,
    char* command, 
    uint32_t command_size,
    std::vector<std::string>& datas);

server_error_t enclave_build_result(
    char* time,
    char* pk,
    char* type,
    uint8_t* payload,
    uint32_t payload_size,
    char* permissions,
    uint32_t permissions_size,
    uint8_t* result,
    uint32_t* p_result_size);

#endif