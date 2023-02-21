/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Description: send data for publishing
 */

#ifndef _CLIENT_PUBLISH_H_
#define _CLIENT_PUBLISH_H_ 

#include <stdio.h>
#include "client.h"

// Send data for publication
int send_data_for_publication(char* time, char* pk, char* type, uint8_t* enc_data, uint32_t enc_data_size);

// Receive plaintext data, encrypt and send to server for publishing
int client_publish(uint8_t* key, client_data_t data);

#endif