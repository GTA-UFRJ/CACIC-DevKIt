/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Description: send message quering some data
 */

#ifndef _CLIENT_QUERY_H_
#define _CLIENT_QUERY_H_ 

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Separate size and ecnrypted data from message received
int parse_server_response(char* msg, uint8_t* enc_message, uint32_t* size);

// Send message quering some data
int send_query_message(uint32_t data_index, 
                       uint8_t* enc_message, 
                       uint32_t* enc_message_size,
                       char* command, 
                       uint32_t command_size,
                       uint8_t* enc_pk,
                       char* id);

// Receive query message and send to server 
int client_query(uint8_t* key, uint8_t* data, uint32_t data_index, char* command, uint32_t* data_size, char* id);

#endif