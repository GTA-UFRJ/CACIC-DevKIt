/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Description: send registration info for the server
 */

#ifndef _CLIENT_REGISTER_H_
#define _CLIENT_REGISTER_H_ 

#include <stdio.h>
#include "client.h"

// Send data for publication
int send_registration(char* snd_msg);

/*
    Build registration message and send to server

    Parameters:
    client_identity_t rcv_id: structure defined in client.h with the ID and CK

    Return:
    Numeric code indicating the error accordingly to errors.h 
*/
int client_register(client_identity_t rcv_id);

#endif