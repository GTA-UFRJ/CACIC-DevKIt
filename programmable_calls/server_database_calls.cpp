/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: call use case database functionalities 
 */

#include "server_database_calls.h"
#include <stdio.h>
#include "server_database_manager.h"
#include "utils.h"
#include <string.h>
#include <sqlite3.h>

server_error_t publish_db(char* time, char* pk, char* type, char* data, uint32_t data_size) 
{
    iot_message_t data_for_writing;
    memcpy(data_for_writing.time, time, 20);
    memcpy(data_for_writing.pk, pk, 9);
    sprintf(data_for_writing.type, type);
    data_for_writing.encrypted_size = data_size; // does not includes /n
    data_for_writing.encrypted = (uint8_t*)malloc(data_size);
    memcpy(data_for_writing.encrypted, data, data_size);
    
    // Database connection 
    sqlite3 *db;
    
    if(sqlite3_open(DATABASE_PATH, &db)) {
       printf("SQL error: %s\n", sqlite3_errmsg(db));
       return print_error_message(OPEN_DATABASE_ERROR);
    } 

    server_error_t ret = database_write(db, data_for_writing);
    free(data_for_writing.encrypted);
    return ret;
}

server_error_t query_db(char* command, uint32_t index, char* data, uint32_t* p_data_size) 
{
    int ret = 0;
    
    char** datas = (char**)malloc(MAX_NUM_DATAS_QUERIED*sizeof(char*)); 
    uint32_t* datas_sizes = (uint32_t*)malloc(MAX_NUM_DATAS_QUERIED*sizeof(uint32_t)); 
    uint32_t filtered_data_count = 0;

    // Database connection 
    sqlite3 *db;
    
    if(sqlite3_open(DATABASE_PATH, &db)) {
       printf("SQL error: %s\n", sqlite3_errmsg(db));
       return print_error_message(OPEN_DATABASE_ERROR);
    } 

    if(ret = database_read(db, command, datas, datas_sizes, &filtered_data_count)) {
        free_data_array(datas, datas_sizes, filtered_data_count);
        return print_error_message(DB_SELECT_EXECUTION_ERROR);
    }

    // Get data at index
    if(index >= filtered_data_count) {
        free_data_array(datas, datas_sizes, filtered_data_count);
        return print_error_message(OUT_OF_BOUND_INDEX);
    }

    memcpy(data, datas[index], datas_sizes[index]);
    free_data_array(datas, datas_sizes, filtered_data_count);

    return OK;
}

server_error_t multi_query_db(char* command, char** datas, uint32_t* datas_sizes, uint32_t* p_data_count) {
    int ret = 0;

    // Database connection 
    sqlite3 *db;
    
    if(sqlite3_open(DATABASE_PATH, &db)) {
       printf("SQL error: %s\n", sqlite3_errmsg(db));
       return print_error_message(OPEN_DATABASE_ERROR);
    } 

    if(ret = database_read(db, command, datas, datas_sizes, p_data_count)) {
        free_data_array(datas, datas_sizes, *p_data_count);
        return print_error_message(DB_SELECT_EXECUTION_ERROR);
    }
    return OK;
}