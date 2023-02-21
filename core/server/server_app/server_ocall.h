/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: enclave calls a bridge function that exits the enclave and call these functions
 */

#ifndef _SERVER_OCALL_
#define _SERVER_OCALL_

#include <stdint.h>
#include "errors.h"

void ocall_print_secret(uint8_t* secret, uint32_t secret_size);
 
void ocall_print_number(long number);

void ocall_print_string(const char* text);

void ocall_query_db(int* p_ret, char* command, uint32_t index, char* data, uint32_t* p_data_size);

void ocall_multi_query_db(int* p_ret, char* command, char** datas, uint32_t* datas_sizes, uint32_t* p_data_count);

#endif