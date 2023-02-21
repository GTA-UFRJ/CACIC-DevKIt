/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: receive client key and write to disk
 */


#ifndef _SERVER_REGISTER_
#define _SERVER_REGISTER_

#include <stdio.h>
#include "sgx_eid.h"
#include "config_macros.h" 
#include "server.h"
#include "errors.h"
#include HTTPLIB_PATH

// Parse request string and fill fields
server_error_t parse_register(char* msg, register_message_t* p_rcv_msg);

// Get register message sent by HTTP header
server_error_t get_register_message(const httplib::Request& req, char* snd_msg, uint32_t* p_size);

// Seal the client key
server_error_t seal_key(register_message_t rcv_msg, sgx_enclave_id_t global_eid, char* path);

// Write client key to disk
server_error_t server_register(const httplib::Request& req, httplib::Response& res, sgx_enclave_id_t global_eid);

#endif