/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: auxiliary funtions and types
 * 
 * This code was modified following access permissions defined
 * by Intel Corporation license, presented as follows
 * 
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
//#include "sample_libcrypto.h"

#ifndef _UTILS_H_
#define _UTILS_H_

#ifndef _ERRNO_T_DEFINED
#define _ERRNO_T_DEFINED
typedef int errno_t;
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(ptr) {if (NULL != (ptr)) {free(ptr); (ptr) = NULL;}}
#endif

errno_t memcpy_s(void *dest, size_t numberOfElements, const void *src,
                 size_t count);

void PRINT_BYTE_ARRAY(FILE *file, void *mem, uint32_t len);

void debug_print_encrypted(size_t , uint8_t* );

void free_data_array(char** , uint32_t* , uint32_t );

bool verify_file_existance(char* );

void free_permissions_array(char**, uint32_t);

void gen_random_index(char* );

void get_time(char* );

int convert_text_to_buffer(char* text, uint32_t text_size, uint8_t* buffer, uint32_t* p_buffer_size);

int convert_buffer_to_text(uint8_t* buffer, uint32_t buffer_size, char* text, uint32_t* p_text_size);

#endif // _UTILS_H_
