/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: process in enclave client data before publishing
 */

#ifndef _SERVER_PUBLISH_
#define _SERVER_PUBLISH_

#include "sgx_eid.h"
#include "config_macros.h" 
#include HTTPLIB_PATH
#include "server.h"

// Parse request string and fill fields
server_error_t parse_request(char* msg, iot_message_t* p_rcv_msg);

// Get publish message sent by HTTP header
server_error_t get_publish_message(const httplib::Request& req, char* snd_msg, uint32_t* p_size);

// Get data, process it and write to database
server_error_t server_publish(const httplib::Request& req, httplib::Response& res, sgx_enclave_id_t global_eid);

#endif