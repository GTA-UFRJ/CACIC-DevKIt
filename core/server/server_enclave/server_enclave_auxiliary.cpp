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
    sgx_aes_ctr_128bit_key_t aes_key;
    memcpy(aes_key, key, 16);

    uint8_t aes_ctr_iv[16] = {0};
    memcpy(enc_data+32, aes_ctr_iv, 16);

    sgx_status_t ret = SGX_SUCCESS;
    
    /* 
    * Encrypted data:      | MAC | IV | AES128(data)
    * Buffer size:           32    16   size(data)
    *
    * MAC reference:         &data       :   &data+32
    * IV reference:          &data+32    :   &data+32+16
    * AES128(data) ref:      &data+32+16 : 
    */
    ret = sgx_aes_ctr_encrypt
        (&aes_key,
        plain_data,
        plain_data_size,
        &aes_ctr_iv[0],
        128,
        enc_data+32+16);

    if(ret != SGX_SUCCESS)
        return ret;

    uint8_t data_to_be_hashed[plain_data_size+16];
    memcpy(data_to_be_hashed, plain_data, plain_data_size);
    memcpy(data_to_be_hashed+plain_data_size, enc_data+32, 16);

    sgx_sha256_hash_t mac = {0};
    ret = sgx_sha256_msg
        (data_to_be_hashed,
        plain_data_size+16,
        &mac);

    memcpy(enc_data,mac,32);

    return ret;
}

// Decryption wrapper
sgx_status_t enclave_decrypt_data(    
    uint8_t* key,
    uint8_t* enc_data,
    uint32_t enc_data_size,
    uint8_t* plain_data)
{
    sgx_aes_ctr_128bit_key_t aes_key;
    memcpy(aes_key, key, 16);

    uint32_t plain_data_size = enc_data_size-32-16;
    memset(plain_data, 0, plain_data_size);

    sgx_status_t ret = SGX_SUCCESS;

    /* 
    * Encrypted data:      | MAC | IV | AES128(data)
    * Buffer size:           32    16   size(data)
    *
    * MAC reference:         &data       :   &data+32
    * IV reference:          &data+32    :   &data+32+16
    * AES128(data) ref:      &data+32+16 : 
    */

    //ocall_print_secret(enc_data, enc_data_size); //ok

    uint8_t iv [16];
    memcpy(iv, enc_data+32, 16);

    ret = sgx_aes_ctr_decrypt
        (&aes_key,
        enc_data+32+16,
        plain_data_size,
        enc_data+32,
        128,
        plain_data);
    
    if(ret != SGX_SUCCESS)
        return ret;

    //ocall_print_string((char*)plain_data);

    uint8_t data_to_be_hashed[plain_data_size+16];
    memcpy(data_to_be_hashed, plain_data, plain_data_size);
    memcpy(data_to_be_hashed+plain_data_size, iv, 16);

    //ocall_print_string((const char*)data_to_be_hashed); //ok
    //ocall_print_secret(data_to_be_hashed,plain_data_size+16); //ok

    sgx_sha256_hash_t hash = {0};
    ret = sgx_sha256_msg
        (data_to_be_hashed,
        plain_data_size+16,
        &hash);
    
    if(ret != SGX_SUCCESS)
        return ret;

    int different = memcmp(hash, enc_data, 32);
    if(different != 0)
        return SGX_ERROR_MAC_MISMATCH;

    return ret;
}

server_error_t enclave_get_payload(
    uint8_t* decrypted, 
    uint32_t decrypted_size, 
    char* payload, 
    uint32_t* payload_size) 
{
    // Verify if payload exists and is valid
    // time|2012-05-06.21:47:59|pk|72d41281|type|123456|fw|654321|vn|789101|payload|...|permission1|72d41281
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
 
        if (i == 11) {
            if(strlen(token) > *payload_size) {
                free(text);
                return INVALID_PAYLOAD_ERROR;
            }
            *payload_size = (uint32_t)strlen(token);
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
    // time|2012-05-06.21:47:59|pk|72d41281|type|123456|fw|654321|vn|789101|payload|...|permission1|72d41281
    char text [1+(size_t)decrypted_len];
    memcpy(text, decrypted, decrypted_len);
    text[decrypted_len] = '\0';
    
    int permission_count = 0;

    int i = 0;
    char* consumed_text = (char*)text;
    char* token = strtok_r(consumed_text, "|", &consumed_text);
    while (token != NULL && *accepted == false)
    {
        i++;
        token = strtok_r(NULL, "|", &consumed_text);
 
        if (i == 11+2*permission_count) {
            if(!memcmp(token, pk, 8))
                *accepted = true;
            permission_count++;
        }
    }

    return OK;
}

server_error_t enclave_get_encrypted(
    char* data, 
    uint32_t , 
    uint8_t* encrypted, 
    uint32_t* p_encrypted_size) 
{
    /*
    char test[70];
    memcpy(test, data, 69);
    test[69] = 0;
    ocall_print_string((const char*)test);
    */

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

    // time|2012-05-06.21:47:59|pk|72d41281|type|123456|fw|654321|vn|789101|payload|...|permission1|72d41281
    int i = 0;
    char* p_access_permissions = &access_permissions[0];
    char* token = strtok_r(p_access_permissions, "|", &p_access_permissions);
    while (token != NULL && i<11)
    {
        token = strtok_r(NULL, "|", &p_access_permissions);
        i++;
    }

    if(p_access_permissions == NULL)
        return EMPTY_PERMISSIONS_ERROR;
    
    if(*p_permissions_size < strlen(p_access_permissions))
        return RESULT_BUFFER_OVERFLOW_ERROR;
    
    *p_permissions_size = (uint32_t)strlen(p_access_permissions);
    strncpy(permissions, p_access_permissions, *p_permissions_size);

    return OK;
}

server_error_t enclave_query_db(
    char* pk,
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

    uint32_t plain_data_size = encrypted_size - 32 - 16;
    if(plain_data_size+1 > *p_data_size)
        return RESULT_BUFFER_OVERFLOW_ERROR;
    uint8_t plain_data[plain_data_size];

    sgx_status_t sgx_ret = enclave_decrypt_data(key, encrypted, encrypted_size, plain_data);
    if(sgx_ret != SGX_SUCCESS) 
        return DATA_DECRYPTION_ERROR;

    bool accepted = false;
    ret = (int) enclave_verify_permissions(plain_data, plain_data_size, pk, &accepted);
    if(ret) 
        return (server_error_t)ret;
    
    if(!accepted)
        return ACCESS_DENIED;
    
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
    uint32_t max_data_count = 10;
    char** stored_datas = (char**)malloc(sizeof(char*)*max_data_count);
    uint32_t* stored_datas_sizes = (uint32_t*)malloc(sizeof(uint32_t)*max_data_count); 

    // OCALL 
    int ret = (int)OK; 
    uint32_t data_count = 0;
    ocall_multi_query_db(&ret, command, command_size, stored_datas, stored_datas_sizes, &data_count);
    if(ret) return (server_error_t)ret;

    uint32_t max_encrypted_size = 1024;
    uint32_t max_plain_data_size = 1024; 

    uint32_t encrypted_size = max_encrypted_size;
    uint8_t encrypted[max_encrypted_size];
    memset(encrypted, 0, max_encrypted_size);

    uint32_t plain_data_size = max_plain_data_size;
    uint8_t plain_data[max_plain_data_size];
    memset(plain_data, 0, max_plain_data_size);

    bool accepted = false;
    data_count = data_count > max_data_count ? max_data_count : data_count;

    for(unsigned i=0; i<data_count; i++) {

        ret = (int) enclave_get_encrypted(stored_datas[i], stored_datas_sizes[i], encrypted, &encrypted_size);
        if(ret) 
            return (server_error_t)ret;

        plain_data_size = encrypted_size - 32 - 16;
        if(plain_data_size > max_plain_data_size) 
            return RESULT_BUFFER_OVERFLOW_ERROR;

        sgx_status_t sgx_ret = enclave_decrypt_data(key, encrypted, encrypted_size, plain_data);
        if(sgx_ret != SGX_SUCCESS) 
            return DATA_DECRYPTION_ERROR;

        if(memcmp(stored_datas[i]+40, plain_data+28, 8)) 
            return DATA_VALIDITY_ERROR;

        //ocall_print_number((long)plain_data_size);
        //ocall_print_string((const char*)plain_data);
        
        ret = (int) enclave_verify_permissions(plain_data, plain_data_size, pk, &accepted);
        if(ret) 
            return (server_error_t)ret;

        if(accepted)
            datas.push_back(std::string((char*)plain_data));
        
        encrypted_size = max_encrypted_size;
        plain_data_size = max_plain_data_size;
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
    char* fw,
    char* vn,
    uint8_t* payload,
    uint32_t payload_size,
    char* permissions,
    uint32_t permissions_size,
    uint8_t* result,
    uint32_t* p_resullt_size) 
{
    if(77+permissions_size+1 > *p_resullt_size) 
        return RESULT_BUFFER_OVERFLOW_ERROR;

    char str[] = "time|xxxxxxxxxxxxxxxxxxx|pk|xxxxxxxx|type|xxxxxx|fw|xxxxxx|vn|xxxxxx|payload|";
    memcpy(&str[5], time, 19);
    memcpy(&str[28], pk, 8);
    memcpy(&str[42], type, 6);
    memcpy(&str[52], fw, 6);
    memcpy(&str[62], vn, 6);
    
    memset(result, 0, *p_resullt_size);
    memcpy(result, &str[0], 77);

    memcpy(result+77, payload, payload_size);
    *(result + 77 + payload_size) = '|';
    memcpy(result + 77 + payload_size + 1, permissions, permissions_size);
    *p_resullt_size = (uint32_t)strlen((char*)result);

    return OK;
}
