/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: enclave calls a bridge function that exits the enclave and call these functions
 */

#include <stdio.h>
#include <stdlib.h>
#include "server_database_calls.h"
#include <stdint.h>
#include "errors.h"

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
    uint32_t , 
    char** datas, 
    uint32_t* datas_sizes, 
    uint32_t* p_data_count) 
{
    *p_ret = (int) multi_query_db(command, datas, datas_sizes, p_data_count);
}