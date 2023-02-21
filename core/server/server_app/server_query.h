/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: query message and return for client
 */

#ifndef _SERVER_QUERY_
#define _SERVER_QUERY_

#include <stdio.h>
#include "sgx_eid.h"
#include "config_macros.h" 
#include "server.h"
#include "errors.h"
#include HTTPLIB_PATH

// Parse request string and fill fields
server_error_t parse_query(char* msg, access_message_t* p_rcv_msg);

// Get query message sent by HTTP header
server_error_t get_query_message(const httplib::Request& req, char* snd_msg, uint32_t* p_size);

// Re-encrypt the data, now using querier key instead of publisher key
server_error_t get_response(stored_data_t stored, 
                            sgx_enclave_id_t global_eid, 
                            uint8_t* response, 
                            access_message_t rcv_msg);

// Mount HTTP response for client
void make_response(uint8_t* enc_data, uint32_t enc_data_size, char* response);

// Get data, process it and read to database
server_error_t server_query(const httplib::Request& req, httplib::Response& res, sgx_enclave_id_t global_eid);

#endif