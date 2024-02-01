# Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
# Author: Guilherme Araujo Thomaz
# Descripton: auxiliary functions for manipualting files

from scacic_macros import SEALS_PATH
from scacic_utils import *
from os import environ
from scacic_errors import Server_error

def get_ca_key_env():
    ca_text = environ.get('CA', None)
    if ca_text is None:
        print("Fatal Error: CA environment variable not encountered")
        exit(1) 
    return convert_text_to_bytes(ca_text)

# Assumes authomatic unsealing at f.read()
def get_ca_key():
    ca_path = SEALS_PATH + "/storage_key_container"
    try:
        with open(ca_path, 'rb') as f:
            ca = f.read()
    except Exception:
       return None, Server_error.print_error(Server_error.OPEN_SERVER_KEY_FILE_ERROR)
    return ca, Server_error.OK

def read_enc_ck_file(id):
    ck_path = SEALS_PATH + "/" + id + "_container"
    try:
        with open(ck_path, 'rb') as f:
            encrypted_ck = f.read()
    except Exception:
       return None, Server_error.print_error(Server_error.OPEN_CLIENT_KEY_FILE_ERROR)
    return encrypted_ck, Server_error.OK

def get_ck_key(id):
    error_code = Server_error.OK

    encrypted_ck, error_code = read_enc_ck_file(id)
    if(error_code != Server_error.OK):
        return None, error_code

    ca, error_code = get_ca_key()
    if(error_code != Server_error.OK):
        return None, error_code
    
    plain_ck, error_code = decrypt(encrypted_ck, ca)
    
    return plain_ck, error_code
    #return ca, error_code #debug
    