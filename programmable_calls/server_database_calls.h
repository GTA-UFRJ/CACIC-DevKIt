/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: call use case database functionalities 
 */

#ifndef _SERVER_DATABASE_CALLS_
#define _SERVER_DATABASE_CALLS_

#include <sqlite3.h>
#include "config_macros.h"
#include "errors.h"

server_error_t publish_db(char* time, char* pk, char* type, char* data, uint32_t data_size);
server_error_t query_db(char* command, uint32_t index, char* data, uint32_t* p_data_size);
server_error_t multi_query_db(char* command, char** datas, uint32_t* datas_sizes, uint32_t* p_data_count);

#endif