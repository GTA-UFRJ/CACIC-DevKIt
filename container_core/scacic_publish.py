# Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
# Author: Guilherme Araujo Thomaz
# Descripton: perform security checks before calling task

import scacic_macros
from scacic_errors import *
from scacic_tasks_calls import *
from scacic_disk_manager import *
from scacic_utils import *
from scacic_databse_calls import *

class Publication:

    def __init__(self, message):
        self.error = Server_error.OK
        self.parse_fields(message)
        if(self.succeeded()):
            self.set_encrypted_bytes()
            self.set_task()
        
    def parse_fields(self, message):
        fields_list = message.split('/')[3].split('|')
        if(len(fields_list) != 10):
            self.error = Server_error.print_error(Server_error.INVALID_ENCRYPTED_FIELD_ERROR)
        self.time, self.id, self.type, self.enc_size, self.enc_text = [fields_list[index] for index in range(len(fields_list)) if index % 2 != 0]
    
    def set_encrypted_bytes(self):
        self.enc_bytes = bytes.fromhex(self.enc_text.replace('-',''))

    def set_task(self):  
        if(self.type not in server_functions.keys()):
            self.error = Server_error.print_error(Server_error.INVALID_ENCRYPTED_FIELD_ERROR)
        self.task_function = server_functions[self.type]

    def succeeded(self):
        return (self.error == Server_error.OK)

    # time|...|pk|...|type|...|payload|...|permission1|...|permission2|...
    def parse_fields_decrypted(self, decrypted):
        fields_list = decrypted.split('|')
        if(len(fields_list) < 10):
            self.error = Server_error.print_error(Server_error.EMPTY_PERMISSIONS_ERROR)
            return
        retrieved_id, self.received_payload = [fields_list[index] for index in [3,7]]
        if(self.id != retrieved_id):
            self.error = Server_error.print_error(Server_error.AUTHENTICATION_ERROR)
        self.permissions_list = [fields_list[index] for index in range(9,len(fields_list)) if index % 2 != 0]
        
    def encrypt_result(self, plain_result):
        ca = get_ca_key()
        self.encrypted_result, self.error = encrypt(plain_result, ca)

    def build_result(self):
        prefix_list = ['permissions{}'.format(i+1) for i in range(len(self.permissions_list))]
        permissions_str = '|'.join([elem for pair in zip(prefix_list, self.permissions_list) for elem in pair])
        plain_result = "time|{}|pk|{}|type|{}|payload|{}|{}".format(self.time, self.pk, self.type, self.result_payload, permissions_str)
        self.encrypt_result(plain_result)

    def publish_result(self):
        self.error = db_publish(self.time, self.id, self.type, self.encrypted_result)
        
    def publication_request_exec(self):

        self.ck, self.error = get_ck_key(self.id)
        if(self.error != Server_error.OK):
            return

        decrypted, self.error = decrypt(self.enc_bytes, self.ck)
        if(self.error != Server_error.OK):
            return

        self.parse_fields_decrypted(decrypted)
        if(self.error != Server_error.OK):
            return

        self.result_payload, self.error = self.task_function(self.time, self.id, self.received_payload, self.ck)
        if(self.error != Server_error.OK):
            return

        self.build_result()
