/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Description: command line interface 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <cstdio>  
#include "client_permdb_manager.h"
#include "client_key_manager.h"
#include "client_publish.h"
#include "client_query.h"
#include "client_register.h"
#include "client_generic_interface.h"
#include "config_macros.h" 
#include "utils.h" 
#include "errors.h"
#include "client.h"

int main (int argc, char *argv[]) {

    if(argc < 2) { 
        printf("Too less arguments\n");
        print_usage();
        return -1;
    }

    if (!strcmp(argv[1],"publish"))
        return publish_interface(argc, argv);

    else if (!strcmp(argv[1],"query"))
        return query_interface(argc, argv);
        
    else if (!strcmp(argv[1],"read_perm"))
        return read_perm_interface(argc, argv);

    else if (!strcmp(argv[1],"write_perm"))
        return write_perm_interface(argc, argv);

    else if (!strcmp(argv[1],"register")) 
        return register_interface(argc, argv);
    
    else {
        printf("Invalid message\n");
        print_usage();
        return -1;
    }
    
    return 0;
}