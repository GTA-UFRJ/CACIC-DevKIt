/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: call use case database functionalities 
 */

#ifndef _SERVER_DATABASE_CALLS_
#define _SERVER_DATABASE_CALLS_

#include "config_macros.h"
#include "errors.h"

/*
    Program this interface for publishing data in the database

    Parameters:
    char* time: input array with size 20 containing publication timestamp
    char* pk: input array with 8 hex characters client ID + end of line character 
    char* type: input array with 6 decimal characters type code + end of line character 
    char* data: input array with size equals to data_size containing encrypted data
    uint32_t data_size: size of data array

    Return:
    Enumerated type defined in errors.h 
*/
server_error_t publish_db(char* time, char* pk, char* type, char* data, uint32_t data_size);

/*
    Program this interface for queriying data in the database

    Parameters:
    char* command: input array with the command for locating the data
    uint32_t index: numeric index for locating data
    char* data: output buffer with the following format:
        time|[19 chars]|type|[6 chars]|pk|[8 chars]|size|0x[size]|encrypted|["size" bytes]
    uint32_t* p_data_size: output with size of data array 

    Return:
    Enumerated type defined in errors.h 
*/
server_error_t query_db(char* command, uint32_t index, char* data, uint32_t* p_data_size);

/*
    Program this interface for queriying multiple data in the database

    Parameters:
    char* command: input array with the command for locating the data
    char** datas: output array of datas. Each data contains the following format:
        time|[19 chars]|type|[6 chars]|pk|[8 chars]|size|0x[size]|encrypted|["size" bytes]
    uint32_t* datas_sizes: output array of size *p_data_count with datas sizes
    uint32_t* p_data_cout: number of elements in datas and datas_sizes arrays 

    Return:
    Enumerated type defined in errors.h 
*/
server_error_t multi_query_db(char* command, char** datas, uint32_t* datas_sizes, uint32_t* p_data_count);

#endif