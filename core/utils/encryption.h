/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Description: high level data encryption and decryption wrappers
 */

#ifndef ENCRYPTION_H_
#define ENCRYPTION_H_

#include <stdlib.h>
#include <stdio.h>

int encrypt_data (uint8_t* ,uint8_t* , uint32_t* , uint8_t* , uint32_t );
int decrypt_data (uint8_t* ,uint8_t* , uint32_t , uint8_t* , uint32_t* );

void quick_decrypt_debug (uint8_t* , uint8_t* , uint32_t );

#endif