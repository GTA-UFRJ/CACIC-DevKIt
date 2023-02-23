/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: utilities for using inside enclave
 */

#include "server_enclave_auxiliary.h"
#include "string.h"
#include "sgx_tcrypto.h"
#include "server_enclave_t.h"
#include <vector>

// Implemented in "server_enclave_t.h"
// void ocall_query_db(int* p_ret, char* command, uint32_t index, char* data, uint32_t* p_data_size);
// void ocall_multi_query_db(int* p_ret, char* command, char** datas, uint32_t* datas_sizes, uint32_t* p_data_count);

// Encryption wrapper
sgx_status_t enclave_encrypt_data(    
    uint8_t* key,
    uint8_t* enc_data,
    uint8_t* plain_data,
    uint32_t plain_data_size)
{
    sgx_aes_gcm_128bit_key_t aes_key;
    memcpy(aes_key, key, 16);

    uint8_t aes_gcm_iv[12] = {0};
    memcpy(enc_data+16, aes_gcm_iv, 12);

    /* 
    * Encrypted data:      | MAC | IV | AES128(data)
    * Buffer size:           16    12   size(data)
    *
    * MAC reference:         &data       :   &data+16
    * IV reference:          &data+16    :   &data+16+12
    * AES128(data) ref:      &data+12+16 : 
    */
    sgx_status_t ret = sgx_rijndael128GCM_encrypt
        (&aes_key,
        plain_data,
        plain_data_size,
        enc_data + 16 + 12,
        &aes_gcm_iv[0],
        12,
        NULL,
        0,
        (sgx_aes_gcm_128bit_tag_t*)enc_data);

    return ret;
}

// Decryption wrapper
sgx_status_t enclave_decrypt_data(    
    uint8_t* key,
    uint8_t* enc_data,
    uint32_t enc_data_size,
    uint8_t* plain_data)
{
    sgx_aes_gcm_128bit_key_t aes_key;
    memcpy(aes_key, key, 16);

    memset(plain_data, 0, enc_data_size-12-16);

    /* 
    * Encrypted data:      | MAC | IV | AES128(data)
    * Buffer size:           16    12   size(data)
    *
    * MAC reference:         &data       :   &data+16
    * IV reference:          &data+16    :   &data+16+12
    * AES128(data) ref:      &data+12+16 : 
    */
    sgx_status_t ret = sgx_rijndael128GCM_decrypt
        (&aes_key,
        enc_data + 16 + 12,
        enc_data_size-12-16,
        plain_data,
        enc_data + 16,
        12,
        NULL,
        0,
        (const sgx_aes_gcm_128bit_tag_t*)(&enc_data[0]));

    return ret;
}

server_error_t enclave_get_payload(
    uint8_t* decrypted, 
    uint32_t decrypted_size, 
    char* payload, 
    uint32_t* payload_size) 
{
    // Verify if payload exists and is valid
    // time|2012-05-06.21:47:59|pk|72d41281|type|123456|payload|250|permission1|72d41281
    char* text = (char*)malloc(1+decrypted_size);
    memcpy(text, decrypted, decrypted_size);
    text[decrypted_size] = '\0';

    int i = 0;
    char* auxiliar_text = text;
    char* token = strtok_r(auxiliar_text, "|", &auxiliar_text);
    while (token != NULL)
    {
        i++;
        token = strtok_r(NULL, "|", &auxiliar_text);
 
        if (i == 7) {
            if(strlen(token) > *payload_size) {
                free(text);
                return INVALID_PAYLOAD_ERROR;
            }
            *payload_size = strlen(token);
            memcpy(payload, token, *payload_size+1);
        }
    }
    free(text);

    return OK;
}

server_error_t enclave_verify_permissions(
    uint8_t* decrypted, 
    uint32_t decrypted_len, 
    char* pk, 
    bool* accepted) 
{
    // Get permissions and verify if querier is included
    // time|2012-05-06.21:47:59|pk|72d41281|type|123456|payload|250|permission1|72d41281
    char* text = (char*)malloc(1+(size_t)decrypted_len);
    memcpy(text, decrypted, decrypted_len);
    text[decrypted_len] = '\0';
    
    int permission_count = 0;

    int i = 0;
    char* token = strtok_r(text, "|", &text);
    while (token != NULL && *accepted == false)
    {
        i++;
        token = strtok_r(NULL, "|", &text);
 
        if (i == 9+2*permission_count) {
            if(!memcmp(token, pk, 8))
                *accepted = true;
            permission_count++;
        }
    }
    free(text);
    return OK;
}

server_error_t enclave_get_encrypted(
    char* data, 
    uint32_t , 
    uint8_t* encrypted, 
    uint32_t* p_encrypted_size) 
{
    unsigned i = 0;
    char* invalid_char;
    uint32_t encrypted_size;

    char* token = strtok_r(data, "|", &data);
    while (token != NULL && i<8)
    {
        i++;
        token = strtok_r(NULL, "|", &data);

        // Get encrypted message size
        if (i == 7) {
            encrypted_size = (uint32_t)strtoul(token, &invalid_char, 16);

            if(token == 0 && *invalid_char != 0) 
                return INVALID_ENCRYPTED_SIZE_FIELD_ERROR;
        }
    }
    
    if(encrypted_size > *p_encrypted_size)
        return INVALID_ENCRYPTED_SIZE_FIELD_ERROR;

    *p_encrypted_size = encrypted_size;
    memcpy(encrypted, data, encrypted_size);

    return OK;
}

server_error_t enclave_get_permissions(
    char* plain_data,
    uint32_t plain_data_size,
    char* permissions,
    uint32_t* p_permissions_size)
{
    char access_permissions [1+plain_data_size];
    memcpy(access_permissions, plain_data, plain_data_size);
    access_permissions[plain_data_size] = 0;
    
    int i = 0;
    char* p_access_permissions = &access_permissions[0];
    char* token = strtok_r(p_access_permissions, "|", &p_access_permissions);
    while (token != NULL && i<7)
    {
        token = strtok_r(NULL, "|", &p_access_permissions);
        i++;
    }
    
    if(*p_permissions_size < strlen(p_access_permissions))
        return RESULT_BUFFER_OVERFLOW_ERROR;
    *p_permissions_size = strlen(p_access_permissions);
    strncpy(permissions, p_access_permissions, *p_permissions_size);

    return OK;
}

server_error_t enclave_query_db(
    uint8_t* key,
    char* command,
    uint32_t index,
    char* data,
    uint32_t* p_data_size)
{
    // OCALL
    int ret = (int)OK;
    ocall_query_db(&ret, command, index, data, p_data_size);
    if(ret) return (server_error_t)ret;

    uint32_t encrypted_size = 1024;
    uint8_t encrypted[encrypted_size];
    ret = (int) enclave_get_encrypted(data, *p_data_size, encrypted, &encrypted_size);
    if(ret) return (server_error_t)ret;

    uint32_t plain_data_size = encrypted_size - 12 - 16;
    if(plain_data_size+1 > *p_data_size)
        return RESULT_BUFFER_OVERFLOW_ERROR;
    uint8_t plain_data[plain_data_size];

    sgx_status_t sgx_ret = enclave_decrypt_data(key, encrypted, encrypted_size, plain_data);
    if(sgx_ret != SGX_SUCCESS) 
        return DATA_DECRYPTION_ERROR;
    
    *p_data_size = plain_data_size+1;
    memcpy(data, plain_data, plain_data_size);
    data[plain_data_size] = '\0';
    
    return OK;
}

void enclave_free_data_array(
    char** datas, 
    uint32_t* datas_sizes, 
    uint32_t data_count) 
{
    for(unsigned i = 0; i < data_count; i++)
        free(datas[i]);
    free(datas);
    free(datas_sizes);
}


// There is memory leackage in this function
server_error_t enclave_multi_query_db(
    char* pk,
    uint8_t* key,
    char* command, 
    uint32_t command_size,
    std::vector<std::string>& datas) 
{
    uint32_t max_data_size = 2048; 
    uint32_t max_data_count = 256;
    char** stored_datas = (char**)malloc(sizeof(char*)*max_data_count);
    uint32_t* stored_datas_sizes = (uint32_t*)malloc(sizeof(uint32_t)*max_data_count); 

    // OCALL 
    int ret = (int)OK; 
    uint32_t data_count = 0;
    ocall_multi_query_db(&ret, command, command_size, stored_datas, stored_datas_sizes, &data_count);
    if(ret) return (server_error_t)ret;
    //ocall_print_number((long)data_count);

    uint32_t max_encrypted_size = 1024;
    uint32_t max_plain_data_size = 1024; 

    uint32_t encrypted_size = max_encrypted_size;
    uint8_t encrypted[max_encrypted_size];
    memset(encrypted, 0, max_encrypted_size);

    uint8_t plain_data[max_plain_data_size];
    memset(plain_data, 0, max_plain_data_size);
    uint32_t plain_data_size = max_plain_data_size;

    bool accepted = false;
    data_count = data_count > max_data_count ? max_data_count : data_count;

    for(unsigned i=0; i<data_count; i++) {

        ret = (int) enclave_get_encrypted(stored_datas[i], stored_datas_sizes[i], encrypted, &encrypted_size);
        if(ret) 
            return (server_error_t)ret;

        plain_data_size = encrypted_size - 12 - 16;
        if(plain_data_size > max_plain_data_size) 
            return RESULT_BUFFER_OVERFLOW_ERROR;

        sgx_status_t sgx_ret = enclave_decrypt_data(key, encrypted, encrypted_size, plain_data);
        if(sgx_ret != SGX_SUCCESS) 
            return DATA_DECRYPTION_ERROR;

        if(memcmp(stored_datas[i]+40, plain_data+28, 8)) 
            return DATA_VALIDITY_ERROR;

        ret = (int) enclave_verify_permissions(plain_data, plain_data_size, pk, &accepted);
        if(ret) 
            return (server_error_t)ret;

        if(accepted) {
            datas.push_back(std::string((char*)plain_data));
            //ocall_print_string((const char*)datas[datas.size()-1].c_str());
        }

        memset(encrypted, 0, max_encrypted_size);
        memset(plain_data, 0, max_plain_data_size);
        accepted = false;
    }

    return OK;
}

server_error_t enclave_build_result(
    char* time,
    char* pk,
    char* type,
    uint8_t* payload,
    uint32_t payload_size,
    char* permissions,
    uint32_t permissions_size,
    uint8_t* result,
    uint32_t* p_resullt_size) 
{
    if(57+permissions_size+1 > *p_resullt_size) 
        return RESULT_BUFFER_OVERFLOW_ERROR;

    char str[] = "time|xxxxxxxxxxxxxxxxxxx|pk|xxxxxxxx|type|xxxxxx|payload|";
    memcpy(&str[5], time, 19);
    memcpy(&str[28], pk, 8);
    memcpy(&str[42], type, 6);
    
    memset(result, 0, *p_resullt_size);
    memcpy(result, &str[0], 57);

    memcpy(result+57, payload, payload_size);
    *(result + 57 + payload_size) = '|';
    memcpy(result + 57 + payload_size + 1, permissions, permissions_size);
    *p_resullt_size = strlen((char*)result);

    return OK;
}