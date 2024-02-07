# Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
# Author: Guilherme Araujo Thomaz
# Descripton: perform security checks before querying data

import scacic_macros
from scacic_errors import Server_error
from scacic_tasks_calls import *
from scacic_disk_manager import *
from scacic_utils import *
from scacic_databse_calls import *
from sys import getsizeof

class Query:

    def __init__(self, message):
        self.error = Server_error.OK
        self.parse_fields(message)
        if(self.succeeded()):
            self.enc_bytes = convert_text_to_bytes(self.enc_text)
    
    # pk|72d41281|index|000000|size|23|command|SELECT_*_FROM_TACIOT_WHERE_TYPE='555555'|encrypted|
    def parse_fields(self, message):
        fields_list = message.split('/')[3].split('|')
        if(len(fields_list) != 10):
            self.error = Server_error.print_error(Server_error.INVALID_ENCRYPTED_FIELD_ERROR)
            raise
        self.id, self.index, self.size, self.command, self.enc_text = [fields_list[index] for index in range(len(fields_list)) if index % 2 != 0]
        print_if_debug("Parsed fields: ", self.id, self.index, self.size, self.command, self.enc_text, sep=' ')

    def succeeded(self):
        return (self.error == Server_error.OK)
    
    # time|...|pk|...|type|...|payload|...|permission1|...|permission2|...
    
    def retrieve_data(self):
        self.retrieved_data, self.error = db_query(self.command, self.index)
        if(self.error != Server_error.OK):
            return
        
        ca, self.error = get_ca_key()
        if(self.error != Server_error.OK):
            return

        self.retrieved_data['decrypted_from_db'], self.error = decrypt(self.retrieved_data['encrypted'], ca)
        if(self.error != Server_error.OK):
            return
        
        decrypted_fields, self.error = parse_fields_decrypted(self.retrieved_data['decrypted_from_db'])
        if(self.error != Server_error.OK):
            return 
        self.retrieved_data['payload'], self.retrieved_data['permissions_list'] = decrypted_fields['payload'], decrypted_fields['perms_list']
        
        print_if_debug("Retrieved data: ", self.retrieved_data)

        #if(decrypted_fields['id'] != self.retrieved_data['id']):
        #    self.error = Server_error.print_error(Server_error.INVALID_ENCRYPTED_FIELD_ERROR)

    def allow_access(self):
        return self.id in self.retrieved_data['permissions_list']
    
    def make_response(self, encrypted):
        return "size|0x" + format(getsizeof(encrypted), '02x') + '|encrypted|' + convert_bytes_to_text(encrypted)

    def query_request_exec(self):

        self.ck, self.error = get_ck_key(self.id)
        if(self.error != Server_error.OK):
            raise
        print_if_debug("Retrieved client communication key: ", self.ck)

        decrypted, self.error = decrypt(self.enc_bytes, self.ck)
        decrypted = decrypted.decode()
        if(self.error != Server_error.OK):
            raise
        
        if(not authenticate_client(self.id, decrypted)):
            self.error = Server_error.print_error(Server_error.AUTHENTICATION_ERROR)
            raise
        
        print_if_debug("Client ", self.id, " authenticated!")

        # Bug here
        self.retrieve_data()
        if(self.error != Server_error.OK):
            raise

        if(not self.allow_access()):
            self.error = Server_error.print_error(Server_error.ACCESS_DENIED)
            raise
        
        print_if_debug("Access allowed!")

        encrypted, self.error = encrypt(self.retrieved_data['decrypted_from_db'], self.ck)
        if(self.error != Server_error.OK):
            raise

        self.respone = self.make_response(encrypted)
        print("Response: ", self.respone)
