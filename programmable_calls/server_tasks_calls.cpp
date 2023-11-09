/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: call use case publication requests processing tasks 
 */

#include "server_tasks_calls.h"
#include "server_enclave_auxiliary.h"
#include <string.h>
#include "server_enclave_t.h"
#include <vector>

// Add the types codes and corresponding tasks here
std::vector<const char*> types{"123456"};
std::vector<task_function_t> tasks{NULL};


server_error_t get_task_by_type(char* type, task_function_t* task) {
    for(unsigned i=0; i<types.size(); i++) {
        if(!memcmp(types[i], type, 6)) {
            *task = tasks[i];
            return OK;
        }
    }
    return INVALID_TYPE_ERROR;
}
