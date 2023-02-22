/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: call use case publication requests processing tasks 
 */

#include "server_tasks_calls.h"
#include "server_enclave_auxiliary.h"
#include <string.h>
#include "server_enclave_t.h"

std::vector<const char*> types{"123456", "555555"};
std::vector<task_function_t> tasks{NULL, &aggregation};


server_error_t get_task_by_type(char* type, task_function_t* task) {
    for(unsigned i=0; i<types.size(); i++) {
        if(!memcmp(types[i], type, 6)) {
            *task = tasks[i];
            return OK;
        }
    }
    return INVALID_TYPE_ERROR;
}

// Task #555555
server_error_t aggregation
(char* time,
 char* pk,
 char* payload,             // DB command
 uint32_t payload_size,
 uint8_t* client_key,
 uint8_t* storage_key,
 uint8_t* result,
 uint32_t* p_result_size)
{
    uint32_t max_data_size = 1024; 
    uint32_t max_data_count = 10;
    
    char** datas = (char**)malloc(sizeof(char*)*max_data_count);
    uint32_t* datas_sizes = (uint32_t*)malloc(sizeof(uint32_t)*max_data_count); 
    for(unsigned i = 0; i < max_data_count; i++)
        datas[i] = (char*)malloc(max_data_size);

    uint32_t data_count = 0;
    server_error_t ret = enclave_multi_query_db(pk, storage_key, payload, strlen(payload), datas, datas_sizes, &data_count);
    if(ret) {
        enclave_free_data_array(datas, datas_sizes, max_data_count);
        return ret;
    }

    uint32_t stored_payload_size = 128;
    char stored_payload[stored_payload_size];
    memset(stored_payload, 0, stored_payload_size);

    unsigned long numeric_payload = 0;
    unsigned long total = 0;

    for(unsigned i=0; i<data_count; i++) 
    {
        ret = enclave_get_payload((uint8_t*)(datas[i]), datas_sizes[i], stored_payload, &stored_payload_size); 
        if(ret) {
            enclave_free_data_array(datas, datas_sizes, max_data_count);
            return ret;
        }
        
        char* invalid_char;
        numeric_payload = strtoul(stored_payload, &invalid_char, 10);
                    
        if(stored_payload != 0 && *invalid_char == 0) {
            total += numeric_payload;
        }
    }

    std::string total_string = std::to_string(total);
    *p_result_size = total_string.length();
    memcpy(result, total_string.c_str(), *p_result_size);

    enclave_free_data_array(datas, datas_sizes, max_data_count);
    return OK;
}