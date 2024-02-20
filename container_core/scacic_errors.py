# Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
# Author: Guilherme Araujo Thomaz
# Descripton: errors codes

from scacic_macros import DEBUG

class Server_error:
    OK = 0
    INIT_ERROR = 1
    ENCALVE_INIT_ERROR = 2
    INVALID_HTTP_MESSAGE_SIZE_FIELD_ERROR = 3
    HTTP_MESSAGE_SIZE_OVERFLOW_ERROR = 4
    INVALID_ENCRYPTED_SIZE_FIELD_ERROR = 5
    INVALID_ENCRYPTED_FIELD_ERROR = 6
    OPEN_DATABASE_ERROR = 7
    OPEN_CLIENT_KEY_FILE_ERROR = 8
    OPEN_SERVER_KEY_FILE_ERROR = 9
    MESSAGE_DECRYPTION_ERROR = 10
    AUTHENTICATION_ERROR = 11
    INVALID_DB_STATEMENT_ERROR = 12
    DB_SELECT_EXECUTION_ERROR = 13
    DATA_DECRYPTION_ERROR = 14
    INVALID_PAYLOAD_ERROR = 15
    DATA_ENCRYPTION_ERROR = 16
    GET_DB_STATEMENT_ENCLAVE_ERROR = 17
    SUM_ENCRYPTED_ENCLAVE_ERROR = 18
    DB_INSERT_EXECUTION_ERROR = 19
    ENCRYPTED_OVERFLOW_ERROR = 20
    NO_PROCESSING_ENCLAVE_ERROR = 21
    INVALID_INDEX_FIELD_ERROR = 22
    INVALID_COMMAND_SIZE_FIELD_ERROR = 23
    OUT_OF_BOUND_INDEX = 24
    MESSAGE_ENCRYPTION_ERROR = 25
    RETRIEVE_DATA_ENCLAVE_ERROR = 26
    ACCESS_DENIED = 27
    DB_DELETE_EXECUTION_ERROR = 28
    OWNERSHIP_VIOLATION_ERROR = 29
    REVOKE_DATA_ENCLAVE_ERROR = 30
    INVALID_REGISTRATION_KEY_FIELD_ERROR = 31
    ALREDY_REGISTERED_ERROR = 32
    KEY_REGISTRATION_ERROR = 33
    SEALING_DATA_ENCLAVE_ERROR = 34
    HTTP_SEND_ERROR = 35
    HTTP_RESPONSE_ERROR = 36
    CLIENT_ENCRYPTION_ERROR = 37
    CLIENT_DECRYPTION_ERROR = 38
    INVALID_HTTP_RESPONSE_SIZE_FIELD_ERROR = 39
    HTTP_RESPONSE_SIZE_OVERFLOW_ERROR = 40
    INVALID_ENCRYPTED_RESPONSE_ERROR = 41
    INVALID_ERROR_CODE_FORMAT_ERROR = 42
    AP_READ_PERMS_ERROR = 43
    OPEN_CSV_FILE_ERROR = 44
    WRITE_IDENTITY_FILE_ERROR = 45
    INVALID_TYPE_ERROR = 46
    UNSEAL_CLIENT_KEY_ERROR = 47
    UNSEAL_SERVER_KEY_ERROR = 48
    RESULT_BUFFER_OVERFLOW_ERROR = 49
    PUBLICATION_ENCLAVE_ERROR = 50
    DATA_VALIDITY_ERROR = 51
    EMPTY_RESULT_ERROR = 52
    EMPTY_PERMISSIONS_ERROR = 53
    INFERENCE_TASK_ERROR = 54

    def get_error_message():
        pass

    def print_error(error):
        if(DEBUG and (error != Server_error.OK)):
            print("Error " + str(error))
            switcher = {
                Server_error.INIT_ERROR: "Insuficient arguments",
                Server_error.ENCALVE_INIT_ERROR: "Failed to initialize enclave",
                Server_error.INVALID_HTTP_MESSAGE_SIZE_FIELD_ERROR: "Failed to detect HTTP message size",
                Server_error.HTTP_MESSAGE_SIZE_OVERFLOW_ERROR: "HTTP message bigger than the maximum size",
                Server_error.INVALID_ENCRYPTED_SIZE_FIELD_ERROR: "Invalid encrypted size message format",
                Server_error.INVALID_ENCRYPTED_FIELD_ERROR: "Invalid encrypted message format",
                Server_error.OPEN_DATABASE_ERROR: "Can't open database",
                Server_error.OPEN_CLIENT_KEY_FILE_ERROR: "Failed to open the client file",
                Server_error.OPEN_SERVER_KEY_FILE_ERROR: "Failed to open the server file",
                Server_error.MESSAGE_DECRYPTION_ERROR: "Error decrypting publisher message",
                Server_error.AUTHENTICATION_ERROR: "Invalid encrypted pk. Could not authenticate client",
                Server_error.DATA_ENCRYPTION_ERROR: "Error encrypting data",
                Server_error.ENCRYPTED_OVERFLOW_ERROR: "Insuficient memory for encrypted result",
                Server_error.NO_PROCESSING_ENCLAVE_ERROR: "Enclave problem inside no_processing_s()",
                Server_error.DB_INSERT_EXECUTION_ERROR: "Failed to publish message",
                Server_error.DATA_DECRYPTION_ERROR: "Error decrypting stored data",
                Server_error.INVALID_PAYLOAD_ERROR: "Invalid payload format",
                Server_error.GET_DB_STATEMENT_ENCLAVE_ERROR: "Enclave problem inside get_db_request_s()",
                Server_error.INVALID_DB_STATEMENT_ERROR: "Invalid database statement",
                Server_error.SUM_ENCRYPTED_ENCLAVE_ERROR: "Enclave problem inside sum_encrypted_data_s()",
                Server_error.DB_SELECT_EXECUTION_ERROR: "Failed to query message",
                Server_error.HTTP_SEND_ERROR: "Error sending HTTP message",
                Server_error.HTTP_RESPONSE_ERROR: "Responded with an HTTP error",
                Server_error.CLIENT_ENCRYPTION_ERROR: "Error encrypting message for sending",
                Server_error.CLIENT_DECRYPTION_ERROR: "Error decrypting returned message",
                Server_error.INVALID_HTTP_RESPONSE_SIZE_FIELD_ERROR: "Failed to detect HTTP response size",
                Server_error.HTTP_RESPONSE_SIZE_OVERFLOW_ERROR: "HTTP response bigger than the maximum size",
                Server_error.INVALID_ENCRYPTED_RESPONSE_ERROR: "Invalid encrypted response format",
                Server_error.OUT_OF_BOUND_INDEX: "Index out of bound",
                Server_error.ACCESS_DENIED: "Access denied",
                Server_error.RETRIEVE_DATA_ENCLAVE_ERROR: "Enclave problem inside retrieve_data()",
                Server_error.INVALID_ERROR_CODE_FORMAT_ERROR: "Invalid error code formatation received",
                Server_error.ALREDY_REGISTERED_ERROR: "Alredy registered client ID",
                Server_error.KEY_REGISTRATION_ERROR: "Could not save client key",
                Server_error.AP_READ_PERMS_ERROR: "Could not synchronize with access point",
                Server_error.OPEN_CSV_FILE_ERROR: "Could not open CSV output file",
                Server_error.WRITE_IDENTITY_FILE_ERROR: "Could not save identity",
                Server_error.INVALID_TYPE_ERROR: "Invalid type", 
                Server_error.PUBLICATION_ENCLAVE_ERROR: "Error inside enclave_publication_wrapper",
                Server_error.UNSEAL_CLIENT_KEY_ERROR: "Error unsealing user key",
                Server_error.UNSEAL_SERVER_KEY_ERROR: "Error unsealing server key",
                Server_error.RESULT_BUFFER_OVERFLOW_ERROR: "Insuficient buffer memory to compute result",
                Server_error.DATA_VALIDITY_ERROR: "Invalid data retrived from database",
                Server_error.EMPTY_RESULT_ERROR: "Empty result payload",
                Server_error.EMPTY_PERMISSIONS_ERROR: "Empty permissions error",
            }
            error_message = switcher.get(error, "Unknown error")
            print(error_message)
        return error