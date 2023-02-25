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

typedef 
server_error_t(*task_function_t)
(char*, char*, char*, uint32_t, uint8_t*, uint8_t*, uint8_t*, uint32_t*);

/*
    Program the processing tasts with the following parameters:

    Parameters:
    char* time: input array with size 20 containing publication timestamp
    char* pk: input array with 8 hex characters client ID + end of line character 
    char* payload: input array of size equals to payload_size with publication request
    uint32_t payload_size: size of publication request (payload)
    uint8_t* client_key: input array of size 16 bytes with communication key (CK)
    uint8_t* storage key: input array of size 16 bytes with storage key (SK)
    uint8_t* result: output array of size *result_size with the processing result
    uint32_t* result_size: size of result

    Return:
    Enumerated type defined in errors.h 
*/

server_error_t task_name
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