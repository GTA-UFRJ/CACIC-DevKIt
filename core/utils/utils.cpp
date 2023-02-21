/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: auxiliary funtions and types
 * 
 * This code was modified following access permissions defined
 * by Intel Corporation license, presented as follows
 * 
 */

#include "utils.h"
#include <stdlib.h>
#include <string.h>
//#include "sample_libcrypto.h"
#include "config_macros.h"
#include <unistd.h>
#include <iostream>
#include <chrono>
#include <ctime> 

errno_t memcpy_s(
    void *dest,
    size_t numberOfElements,
    const void *src,
    size_t count)
{
    if(numberOfElements<count)
        return -1;
    memcpy(dest, src, count);
    return 0;
}

void PRINT_BYTE_ARRAY(
    FILE *file, void *mem, uint32_t len)
{
    if(!mem || !len)
    {
        fprintf(file, "\n( null )\n");
        return;
    }
    uint8_t *array = (uint8_t *)mem;
    fprintf(file, "%u bytes:\n{\n", len);
    uint32_t i = 0;
    for(i = 0; i < len - 1; i++)
    {
        fprintf(file, "0x%x, ", array[i]);
        if(i % 8 == 7) fprintf(file, "\n");
    }
    fprintf(file, "0x%x ", array[i]);
    fprintf(file, "\n}\n");
}

//uint8_t iv[12];

void debug_print_encrypted(
    size_t encMessageLen, 
    uint8_t* encMessage){
        printf("Size = %d\nData = ", (int)encMessageLen);
        for (size_t byte=0; byte<encMessageLen; byte++){
            printf("0x%02x, ", encMessage[byte]);
        }
        printf("\n");
}

void free_data_array(char** datas, uint32_t* datas_sizes, uint32_t data_count) {
    for(unsigned i = 0; i < data_count; i++)
        free(datas[i]);
    free(datas);
    free(datas_sizes);
}

bool verify_file_existance(char* filename) {
    if(DEBUG_PRINT) printf("\nVerifiying if file %s exists\n", filename);
    return ( access(filename, F_OK) != -1 ? true : false );
}

void gen_random_index(char* id) 
{
    using namespace std::chrono;
    long long t = time_point_cast<nanoseconds>(system_clock::now()).time_since_epoch().count();
    
    srand((unsigned)t);
    for(int i=0;i<8;i++) 
        id[i] = static_cast<char>(rand()%10 + (int)'0');
    id[8] = '\0';
}

void free_permissions_array(char** permissions_list, uint32_t permissions_count) {
    for(unsigned i = 0; i < permissions_count; i++)
        free(permissions_list[i]);
    free(permissions_list);
}

void get_time(char* result) {
    time_t     now = time(0);
    struct tm  tstruct;
    tstruct = *localtime(&now);
    strftime(result, 20, "%Y-%m-%d.%X", &tstruct);
    result[19] = '\0';
    printf("Current measured time: %s\n", result);
}

int convert_text_to_buffer(char* text, uint32_t text_size, uint8_t* buffer, uint32_t* p_buffer_size) 
{
    uint32_t buffer_size = text_size/3;

    char* invalid_char;
    char auxiliar[3];
    for (uint32_t i=0; i<buffer_size; i++){
        auxiliar[0] = text[3*i];
        auxiliar[1] = text[3*i+1];
        auxiliar[2] = '\0';
        
        buffer[i] = (uint8_t)strtoul(auxiliar, &invalid_char, 16);

        if(auxiliar != 0 && *invalid_char != 0) 
            return -1;
    }

    if(p_buffer_size != NULL) *p_buffer_size = buffer_size;
    return 0;
}

int convert_buffer_to_text(uint8_t* buffer, uint32_t buffer_size, char* text, uint32_t* p_text_size) 
{
    uint32_t text_size = buffer_size*3;

    char auxiliar[4];
    for (uint32_t i=0; i<buffer_size; i++) {
        sprintf(auxiliar, "%02x-", buffer[i]);
        memcpy(&text[3*i], auxiliar, 3);
    }
    text[text_size] = '\0';

    if(p_text_size != NULL) *p_text_size = text_size;
    return 0;
}