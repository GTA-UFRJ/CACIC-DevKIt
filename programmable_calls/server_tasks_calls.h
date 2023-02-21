/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: call use case publication requests processing tasks 
 */

#ifndef _SERVER_TASKS_CALLS_
#define _SERVER_TASKS_CALLS_

#include <string>
#include <vector>
#include "errors.h"

/*
Tasks parameters:
1) char* time
2) char* pk
3) char* payload
4) uint32_t payload_size
5) uint8_t* client_key
6) uint8_t* storage key
7) uint8_t* result
8) uint32_t* result_size
*/
typedef 
server_error_t(*task_function_t)
(char*, char*, char*, uint32_t, uint8_t*, uint8_t*, uint8_t*, uint32_t*);


server_error_t aggregation
(char* time,
 char* pk,
 char* payload,
 uint32_t payload_size,
 uint8_t* client_key,
 uint8_t* storage_key,
 uint8_t* result,
 uint32_t* p_result_size);

server_error_t get_task_by_type(char* type, task_function_t* task);

#endif