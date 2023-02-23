/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: enclave entrypoints
 */
/*
 * Copyright (C) 2011-2020 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <assert.h>
#include "server_enclave_t.h"
#include "sgx_tkey_exchange.h"
#include "sgx_tcrypto.h"
#include "sgx_tseal.h"
#include "sgx_trts.h"
//#include "server_processing.h"
#include <string.h>
#include <string>
#include <stdlib.h>

#include "errors.h"
#include "server.h"
#include "server_tasks_calls.h"
#include "server_enclave_auxiliary.h"

//#include "server_ocall.h"

// Process data before publishing
sgx_status_t enclave_publication_wrapper(
    int debug,
    sgx_sealed_data_t* publisher_sealed_key,
    sgx_sealed_data_t* storage_sealed_key,
    char* time,
    char* pk,
    char* type,
    uint8_t* encrypted,
    uint32_t encrypted_size,
    char* processed_data,
    uint32_t* p_processed_data_size,
    int* p_error_code)
{
    if(debug) ocall_print_string("\n------------ ENTERED ENCLAVE -----------");

    // Verify if nonce is fresh
    // TODO
    *p_error_code = (int)OK;
    sgx_status_t ret = SGX_SUCCESS;

    uint32_t key_size = 16;


    if(debug) ocall_print_string("\nUnsealing publisher key");

    uint8_t publisher_key[16] = {0}; 
    ret = sgx_unseal_data(publisher_sealed_key, NULL, NULL, publisher_key, &key_size);
    if(ret != SGX_SUCCESS) {
        *p_error_code = (int)UNSEAL_CLIENT_KEY_ERROR;
        return ret;
    }
    if(debug) ocall_print_secret(&publisher_key[0], 16);


    if(debug) ocall_print_string("\nUnsealing storage key");

    uint8_t storage_key[16] = {0}; 
    ret = sgx_unseal_data(storage_sealed_key, NULL, NULL, storage_key, &key_size);
    if(ret != SGX_SUCCESS) {
        *p_error_code = (int)UNSEAL_CLIENT_KEY_ERROR;
        return ret;
    }
    if(debug) ocall_print_secret(&storage_key[0], 16);


    if(debug) ocall_print_string("\nDecrypt encrypted field of publication message");

    uint32_t decrypted_size = encrypted_size - 16 - 12;
    uint8_t decrypted [decrypted_size+1];
    ret = enclave_decrypt_data(&publisher_key[0], encrypted, encrypted_size, &decrypted[0]);

    if(ret != SGX_SUCCESS) {
        *p_error_code = (int)MESSAGE_DECRYPTION_ERROR;
        return ret;
    }
    decrypted[decrypted_size] = 0;
    if(debug) ocall_print_string((const char*)decrypted);
    

    if(debug) ocall_print_string("\nVerify if IDs are equal");

    if(memcmp(pk, decrypted+5+20+3, 8)){
        *p_error_code = (int)AUTHENTICATION_ERROR;
        return SGX_SUCCESS;
    }

    if(debug) ocall_print_string("\nGet payload field inside decrypted");

    uint32_t payload_size = 128;
    char payload[payload_size];
    *p_error_code = enclave_get_payload(&decrypted[0], decrypted_size, &payload[0], &payload_size);

    if(*p_error_code != OK)
        return ret;
    if(debug) ocall_print_string((const char*)payload);

    if(debug) ocall_print_string("\nChoose processing task");

    task_function_t task;
    *p_error_code = get_task_by_type(type, &task);

    if(*p_error_code != OK)
        return ret;

    if(debug) ocall_print_string("\nCall task");

    uint32_t result_payload_size = 128;
    uint8_t result_payload[result_payload_size];

    if(task != NULL) {
        *p_error_code = task
            (time, 
             pk, 
             payload, 
             payload_size, 
             publisher_key, 
             storage_key, 
             result_payload, 
             &result_payload_size);
             
        if(*p_error_code != OK)
            return ret;
    }
    
    else {
        result_payload_size = payload_size;
        memcpy(result_payload, payload, payload_size);
    }
    
    if(result_payload_size == 0) {
        *p_error_code = EMPTY_RESULT_ERROR;
        return ret;
    }

    if(debug) ocall_print_string("\nGet access permissions fields inside decrypted");

    uint32_t permissions_size = 255;
    char permissions[permissions_size];
    *p_error_code = enclave_get_permissions((char*)decrypted, decrypted_size, &permissions[0], &permissions_size);

    if(*p_error_code != OK)
        return ret;

    if(debug) ocall_print_string("\nBuild result");

    uint32_t result_size = 1024;
    uint8_t result[result_size];
    *p_error_code = enclave_build_result
        (time, 
         pk, 
         type, 
         &result_payload[0], 
         result_payload_size, 
         permissions,
         permissions_size,
         result, 
         &result_size);

    if(*p_error_code != OK)
        return ret; 
    if(debug) ocall_print_string((const char*) result);

    if(debug) ocall_print_string("\nEcnrypt result");

    *p_processed_data_size = result_size + 16 + 12; // does not inclues /n
    ret = enclave_encrypt_data(storage_key, (uint8_t*)processed_data, &result[0], result_size);

    if(ret != SGX_SUCCESS) {
        *p_error_code = (int)MESSAGE_ENCRYPTION_ERROR;
        return ret;
    }

    return SGX_SUCCESS;
}

sgx_status_t enclave_retrieve_data(
    int debug,
    sgx_sealed_data_t* sealed_querier_key,
    sgx_sealed_data_t* sealed_storage_key,
    uint8_t* encrypted_pk,
    uint8_t* encrypted_data,
    uint32_t encrypted_data_size,
    char* querier_pk,
    uint8_t* result,
    int* p_error_code)
{
     if(debug) ocall_print_string("\n------------ ENTERED ENCLAVE -----------");

    // Verify if nonce is fresh
    // TODO
    *p_error_code = (int)OK;
    sgx_status_t ret = SGX_SUCCESS;

    uint32_t key_size = 16;

    if(debug) ocall_print_string("\nUnsealing querier key");

    uint8_t querier_key[16] = {0}; 
    ret = sgx_unseal_data(sealed_querier_key, NULL, NULL, &querier_key[0], &key_size);
    if(ret != SGX_SUCCESS) {
        *p_error_code = (int)UNSEAL_CLIENT_KEY_ERROR;
        return ret;
    }
    if(debug) ocall_print_secret(&querier_key[0], 16);

    if(debug) ocall_print_string("\nUnsealing storage key");

    uint8_t storage_key[16] = {0}; 
    ret = sgx_unseal_data(sealed_storage_key, NULL, NULL, &storage_key[0], &key_size);
    if(ret != SGX_SUCCESS) {
        *p_error_code = (int)UNSEAL_CLIENT_KEY_ERROR;
        return ret;
    }
    if(debug) ocall_print_secret(&storage_key[0], 16);

    if(debug) ocall_print_string("\nDecrypt encrypted pk field of query message");

    uint32_t decrypted_pk_size = 8;
    uint8_t decrypted_pk [decrypted_pk_size];
    memset(decrypted_pk, 0, decrypted_pk_size);
    ret = enclave_decrypt_data(&querier_key[0], encrypted_pk, 8+16+12, &decrypted_pk[0]);

    if(ret != SGX_SUCCESS) {
        *p_error_code = (int)MESSAGE_DECRYPTION_ERROR;
        return ret;
    }

    if(debug) ocall_print_string("\nVerify if IDs are equal");

    if(memcmp(querier_pk, decrypted_pk, 8)){
        *p_error_code = (int)AUTHENTICATION_ERROR;
        return SGX_SUCCESS;
    }

    if(debug) ocall_print_string("\nDecrypt encrypted stored data");

    // Decrypt encrypted stored data
    uint32_t decrypted_size = encrypted_data_size - 12 - 16;
    uint8_t decrypted [decrypted_size+1];
    ret = enclave_decrypt_data(&storage_key[0], encrypted_data, encrypted_data_size, &decrypted[0]);

    if(ret != SGX_SUCCESS) {
        *p_error_code = (int)MESSAGE_DECRYPTION_ERROR;
        return ret;
    }
    decrypted[decrypted_size] = 0;
    if(debug) ocall_print_string((const char*)decrypted);

    if(debug) ocall_print_string("\nVerify access permissions");

    bool accepted = false;
    *p_error_code = (int) enclave_verify_permissions(decrypted, decrypted_size, querier_pk, &accepted);

    if(*p_error_code != OK)
        return ret;

    if(accepted == false) {
        *p_error_code = (int)ACCESS_DENIED;
        return ret;
    }

    if(debug) ocall_print_string("\nEncrypt data with querier key");

    ret = enclave_encrypt_data(&querier_key[0], result, decrypted, decrypted_size);
    
    if(ret != SGX_SUCCESS) {
        *p_error_code = (int)MESSAGE_DECRYPTION_ERROR;
        return ret;
    }
    
    return ret;
}

sgx_status_t enclave_sealing_data(
    uint8_t* sealed_buffer,
    uint32_t sealed_buffer_size,
    uint32_t* real_sealed_size,
    uint8_t* data,
    uint32_t data_size)
{
    *real_sealed_size = sgx_calc_sealed_data_size(0, data_size);
    if (*real_sealed_size == UINT32_MAX)
        return SGX_ERROR_UNEXPECTED;
    if (*real_sealed_size > sealed_buffer_size)
        return SGX_ERROR_INVALID_PARAMETER;

    sgx_status_t err = sgx_seal_data(0, NULL, data_size, data, *real_sealed_size, (sgx_sealed_data_t *)sealed_buffer);

    return err;
}
