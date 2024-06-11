# Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
# Author: Guilherme Araujo Thomaz
# Descripton: perform security checks before calling task

import scacic_macros
from scacic_errors import Server_error
from scacic_tasks_calls import *
from scacic_disk_manager import *
from scacic_utils import *
from scacic_databse_calls import *

# TODO: substituir códigos de erro por exceções (no query tmb)
class Publication:

    def __init__(self, message):
        self.error = Server_error.OK
        self.parse_fields(message)
        if(self.succeeded()):
            self.enc_bytes = convert_text_to_bytes(self.enc_text)
            self.set_task()
    
    def parse_fields(self, message):
        fields_list = message.split('/')[3].split('|')
        if(len(fields_list) != 14):
            self.error = Server_error.print_error(Server_error.INVALID_ENCRYPTED_FIELD_ERROR)
            raise
        self.time, self.id, self.type, self.fw, self.vn, self.enc_size, self.enc_text = [fields_list[index] for index in range(len(fields_list)) if index % 2 != 0]
        print_if_debug("Parsed fields: ", self.time, self.id, self.type, self.fw, self.vn, self.enc_size, self.enc_text, sep=' ')

    def set_task(self):  
        server_functions = get_server_functions()
        if(self.type not in server_functions.keys()):
            self.error = Server_error.print_error(Server_error.INVALID_ENCRYPTED_FIELD_ERROR)
            raise
        self.task_function = server_functions[self.type]
        print_if_debug("Detected task function: ", self.task_function.__name__)

    def succeeded(self):
        return (self.error == Server_error.OK)

    # time|...|pk|...|type|...|fw|...|vn|...|payload|...|permission1|...|permission2|...
    def parse_fields_decrypted(self, decrypted):
        decrypted_fields, self.error = parse_fields_decrypted(decrypted)
        if(self.error != Server_error.OK):
            return
        self.received_payload, self.permissions_list = decrypted_fields['payload'], decrypted_fields['perms_list']

        if(authenticate_client(self.id, decrypted_fields['id'])):
            print_if_debug("Client ", self.id, " authenticated!")
        else:
            self.error = Server_error.print_error(Server_error.AUTHENTICATION_ERROR)
        
    def encrypt_result(self, plain_result):
        ca, error_code = get_ca_key()
        if(error_code != Server_error.OK):
            return None, error_code
        self.encrypted_result, self.error = encrypt(plain_result.encode(), ca)

        decrypted, self.error = decrypt(self.encrypted_result, ca)
        print(decrypted.decode())

    def build_result(self):
        prefix_list = ['permissions{}'.format(i) for i in range(len(self.permissions_list))]
        permissions_str = '|'.join([elem for pair in zip(prefix_list, self.permissions_list) for elem in pair])
        plain_result = "time|{}|pk|{}|type|{}|fw|{}|vn|{}|payload|{}|{}".format(self.time, self.id, self.type, self.fw, self.vn, self.result_payload, permissions_str)
        print_if_debug("Generated plain result payload: ", plain_result)
        self.encrypt_result(plain_result)

    def publish_result(self):
        self.error = db_publish(self.time, self.id, self.type, self.fw, self.vn, self.encrypted_result)

    def execute_task(self):
        self.result_payload, self.error = self.task_function(self.time, self.id, self.received_payload, self.ck)
        
    def publication_request_exec(self):

        self.ck, self.error = get_ck_key(self.id)
        if(self.error != Server_error.OK):
            raise
        print_if_debug("Retrieved client communication key: ", self.ck)

        decrypted, self.error = decrypt(self.enc_bytes, self.ck)
        decrypted = decrypted.decode()
        if(self.error != Server_error.OK):
            raise
        print_if_debug("Decrypted message: ", decrypted)
        
        self.parse_fields_decrypted(decrypted)
        if(self.error != Server_error.OK):
            raise

        self.execute_task()
        if(self.error != Server_error.OK):
            raise

        self.build_result()
        if(self.error != Server_error.OK):
            raise
