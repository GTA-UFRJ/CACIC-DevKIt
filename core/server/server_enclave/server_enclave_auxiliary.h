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

/*
    Get payload inside decrypted stored data:

    Parameters:
    uint8_t* decrypted: input array of size *decrypted_size with the stored decrypted data
    uint32_t decrypted_size: size of decrypted array
    char* payload: output array of size equals to *payload_size with stored data payload
    uint32_t* payload_size: size of payload array

    Return:
    Enumerated type defined in errors.h 
*/
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

/*
    Get payload inside decrypted stored data:

    Parameters:
    char* pk: input array with 8 hex characters client ID + end of line character 
    uint8_t* key: input array of size 16 bytes with storage key (SK)
    char* command: input array of size command_size locating the data in the database
    uint32_t index: numeric index for locating data
    char* data: output array with queried data 
    uint32_t* p_data_size: output with size of data array

    Return:
    Enumerated type defined in errors.h 
*/
server_error_t enclave_query_db(
    char* pk,
    uint8_t* key,
    char* command,
    uint32_t index,
    char* data,
    uint32_t* p_data_size);

/*
    Get payload inside decrypted stored data:

    Parameters:
    char* pk: input array with 8 hex characters client ID + end of line character 
    uint8_t* key: input array of size 16 bytes with storage key (SK)
    char* command: input array of size command_size locating the data in the database
    uint32_t command_size: size of command array 
    std::vector<std::string>& datas: output vector with strings containing the datas

    Return:
    Enumerated type defined in errors.h 
*/
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
    char* fw,
    char* vn,
    uint8_t* payload,
    uint32_t payload_size,
    char* permissions,
    uint32_t permissions_size,
    uint8_t* result,
    uint32_t* p_result_size);

#endif