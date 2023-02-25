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

/*
    Build query message and send to server 

    Parameters:
    uint8_t* key: input array with 16 characters with the communication key (CK)
    uint8_t* data: output array containing the query result with size equals to *data_size
    uint32_t data: numerical index for locating the data 
    char* command: input array with a command for locating the data
    uint32_t* data_size: output containing the size of data
    char* id: input array with 8 hex characters client ID + end of line character 

    Return:
    Numeric code indicating the error accordingly to errors.h 
*/
int client_query(uint8_t* key, uint8_t* data, uint32_t data_index, char* command, uint32_t* data_size, char* id);

#endif