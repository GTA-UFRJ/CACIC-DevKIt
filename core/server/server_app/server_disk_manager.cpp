/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Description: manages the r/w operations in the database and key vault
 */

#include "server_disk_manager.h"
#include <mutex>
#include <unistd.h>
#include "errors.h"
#include "utils.h"
#include "sgx_tseal.h"
 
std::mutex thread_sync;

server_error_t get_stored_parameters(char* msg, stored_data_t* p_stored)
{
    if(DEBUG_TIMER) Timer t("get_stored_parameters");

    if(DEBUG_PRINT) printf("\nParsing stored data fields\n");

    // time|2012-05-06.21:47:59|type|123456|pk|72d41281|fw|654321|vn|789101|size|0x54|encrypted|0x62-...
    char* token = strtok_r(msg, "|", &msg);
    int i = 0;
    while (token != NULL && i<12)
    {
        i++;
        token = strtok_r(NULL, "|", &msg);

        // Get time     
        if (i == 1) {
            memcpy(p_stored->time, token, 19);
            p_stored->time[19] = '\0';

            if(DEBUG_PRINT) printf("time: %s\n", p_stored->time);
        }

        // Get type     
        if (i == 3) {
            memcpy(p_stored->type, token, 6);
            p_stored->type[6] = '\0';

            if(DEBUG_PRINT) printf("type: %s\n", p_stored->type);
        }

        // Get client pk (id)       
        if (i == 5) {
            memcpy(p_stored->pk, token, 8);
            p_stored->pk[8] = '\0';

            if(DEBUG_PRINT) printf("pk: %s\n", p_stored->pk);
        }

        // Get fw code       
        if (i == 7) {
            memcpy(p_stored->fw, token, 6);
            p_stored->fw[6] = '\0';

            if(DEBUG_PRINT) printf("fw: %s\n", p_stored->fw);
        }

        // Get vn   
        if (i == 9) {
            memcpy(p_stored->vn, token, 6);
            p_stored->vn[6] = '\0';

            if(DEBUG_PRINT) printf("vn: %s\n", p_stored->vn);
        }

        // Get encrypted message size
        if (i == 11) {
            p_stored->encrypted_size = (uint32_t)strtoul(token,NULL,16);
            if(DEBUG_PRINT) printf("encrypted_size: %u\n", p_stored->encrypted_size);
        }
    }

    // Get encrypted
    p_stored->encrypted = (uint8_t*)malloc((p_stored->encrypted_size+1) * sizeof(uint8_t));
    
    memcpy(p_stored->encrypted, msg, p_stored->encrypted_size);
    p_stored->encrypted[p_stored->encrypted_size] = 0;
    //debug_print_encrypted((size_t)(p_stored->encrypted_size), p_stored->encrypted);

    return OK;
}

server_error_t write_key(uint8_t* ck, uint32_t ck_size, char* filename) 
{
    if(DEBUG_TIMER) Timer t("write_key");   
    if(DEBUG_PRINT) printf("\nWriting key into file\n"); 

    // Avoid multiple threads writing at the same time
    thread_sync.lock();

    // Write key to file
    FILE* file = fopen(filename, "ab");
    if (file == NULL) {
        printf("\nFailed to open the key file %s\n", filename);
        fclose(file);
        print_error_message(KEY_REGISTRATION_ERROR);
    }
    fwrite(ck, 1, (size_t)ck_size, file);
    fclose(file);

    // Next thread gets the lock at the start of the function
    thread_sync.unlock();
    
    return OK;
}

server_error_t read_user_key_file(char* pk, uint8_t* key, uint32_t* p_key_size) {

    if(DEBUG_TIMER) Timer t("read_user_key_file");

    // Search user file and read sealed key
    char* querier_seal_path = (char*)malloc(PATH_MAX_SIZE);
    sprintf(querier_seal_path, "%s/ck_%s", SEALS_PATH, pk);

    if(DEBUG_PRINT) printf("\nReading querier key file: %s\n", querier_seal_path);

    FILE* querier_seal_file = fopen(querier_seal_path, "rb");
    free(querier_seal_path);
    if (querier_seal_file == NULL) 
        return print_error_message(OPEN_CLIENT_KEY_FILE_ERROR);
    
    *p_key_size = (uint32_t)(sizeof(sgx_sealed_data_t) + 16);
    fread(key, 1, *p_key_size, querier_seal_file);
    fclose(querier_seal_file);

    return OK;
}


server_error_t read_storage_key_file(char* , uint8_t* key, uint32_t* p_key_size) {

    if(DEBUG_TIMER) Timer t("read_storage_key_file");

    // Search server file and read sealed key
    char* storage_seal_path = (char*)malloc(PATH_MAX_SIZE);
    sprintf(storage_seal_path, "%s/storage_key", SEALS_PATH);

    if(DEBUG_PRINT) printf("\nReading storage key file: %s\n", storage_seal_path);

    FILE* storage_seal_file = fopen(storage_seal_path, "rb");
    free(storage_seal_path);
    if (storage_seal_file == NULL) 
        return print_error_message(OPEN_SERVER_KEY_FILE_ERROR);

    *p_key_size = (uint32_t)(sizeof(sgx_sealed_data_t) + 16);
    fread(key, 1, *p_key_size, storage_seal_file);
    fclose(storage_seal_file);

    return OK;
}