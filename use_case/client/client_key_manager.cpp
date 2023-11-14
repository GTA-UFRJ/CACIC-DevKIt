/*
 * Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
 * Author: Guilherme Araujo Thomaz
 * Description: configure key and ID
 */

#include "client_key_manager.h"
#include "errors.h"
#include <mutex>
#include "utils.h"

std::mutex thread_sync;

int read_identity(client_identity_t* p_id) {

    // Search identity file and read ID and CK
    if(DEBUG_PRINT) printf("\nReading client identity key file: %s\n", CLIENT_KEY_FILENAME);

    FILE* id_file = fopen(CLIENT_KEY_FILENAME, "rb");
    if (id_file == NULL) 
        return (int)print_error_message(OPEN_CLIENT_KEY_FILE_ERROR);

    fread(p_id->pk, 1, 8, id_file);
    (p_id->pk)[8] = '\0';
    fread(p_id->comunication_key, 1, 16, id_file);
    
    fclose(id_file);
    return 0;
}

int write_identity(client_identity_t id) {

    // Avoid multiple threads writing at the same time
    thread_sync.lock();

    // Marshall structure

    // Write key to file
    FILE* file = fopen(CLIENT_KEY_FILENAME, "wb");
    if (file == NULL) {
        printf("\nFailed to open the key file %s\n", CLIENT_KEY_FILENAME);
        fclose(file);
        return (int)WRITE_IDENTITY_FILE_ERROR;
    }
    fwrite(id.pk, 1, 8, file);
    fwrite(id.comunication_key, 1, 16, file);
    fclose(file);

    // Next thread gets the lock at the start of the function
    thread_sync.unlock();
    
    return 0;
}
