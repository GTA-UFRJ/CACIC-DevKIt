/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Descripton: serve client messages and register client
 */

#pragma once

#ifndef CLIENT_PK
#define CLIENT_PK


typedef struct sample_ec_pub_t
{
    uint8_t gx[32];
    uint8_t gy[32];
} sample_ec_pub_t;
#endif

// Structures of the message sent by acess point (used by teh server)
typedef struct iot_message_t
{
    char time[20];
    char pk[9];
    char type[7];
    char fw[7];
    char vn[7];
    uint32_t encrypted_size;
    uint8_t* encrypted;
} iot_message_t;

typedef struct access_message_t
{
    char pk[9];
    uint32_t index;
    uint32_t command_size;
    char* command;
    uint8_t encrypted[8+16+12+1];
} access_message_t;

// Structure of the message written to database
typedef struct stored_data_t
{
    char time[20];
    char pk[9];
    char type[7];
    uint32_t encrypted_size;
    uint8_t* encrypted;
} stored_message_t;

// Structure of the message used to register
typedef struct register_message_t
{
    char pk[9];
    uint8_t ck[16];
} register_message_t;
