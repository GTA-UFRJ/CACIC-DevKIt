# Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
# Author: Guilherme Araujo Thomaz
# Descripton: perform security checks before calling task

import scacic_macros
from scacic_errors import *
from scacic_tasks_calls import *

class Publication:

    def __init__(self, message):
        self.error = Server_error.OK
        self.parse_fields(message)
        if(self.succeeded()):
            self.get_encrypted_buffer()
            self.get_task()
    
    def parse_fields(self, message):
        fields_list = message.split('/')[3].split('|')
        if(len(fields_list) != 10):
            self.error = Server_error.print_error(Server_error.INVALID_ENCRYPTED_FIELD_ERROR)
        self.time, self.id, self.type, self.enc_size, self.enc_text = [fields_list[index] for index in range(len(fields_list)) if index % 2 != 0]
    
    def get_encrypted_buffer(self):
        self.enc_buffer = [int(hex_string,16) for hex_string in self.enc_text.split('-') if hex_string]

    def get_task(self):  
        if(self.type not in server_functions.keys()):
            self.error = Server_error.print_error(Server_error.INVALID_ENCRYPTED_FIELD_ERROR)
        self.task_function = server_functions[self.type]

    def succeeded(self):
        return (self.error == Server_error.OK)

    def publication_request_exec(self):
        pass
