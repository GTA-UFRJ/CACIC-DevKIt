/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Description: wrapper for client backend used from both CLI and GUI applications
 */

#ifndef _CLIENT_GENERIC_INTERFACE_H_
#define _CLIENT_GENERIC_INTERFACE_H_

#include "errors.h"
#include <string>
#include "client.h"

void print_usage();

int publish_interface(int agrc, char** argv);
int query_interface(int argc, char** argv);
int read_perm_interface(int argc, char** argv );
int write_perm_interface(int argc, char** argv );
int register_interface(int argc, char** argv );

//ap_init
#endif
