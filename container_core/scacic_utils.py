# Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
# Author: Guilherme Araujo Thomaz
# Descripton: auxiliary functions for encryption, etc

from Crypto.Cipher import AES
from scacic_macros import *
from scacic_errors import *
import os
from scacic_databse_calls import *

def separare_fields_encrypted(encrypted_data):
    if(len(encrypted_data) <= 16+12):
        return None, None, None, Server_error.print_error(Server_error.INVALID_ENCRYPTED_FIELD_ERROR)
    mac, iv, data = encrypted_data[:16], encrypted_data[16:16+12], encrypted_data[16+12:]
    return mac, iv, data, Server_error.OK

def build_fields_encrypted(mac, data, nonce):
    return mac + nonce + data

def encrypt(plaintext, key):
    try:    
        encobj = AES.new(key, AES.MODE_GCM)
        ciphertext, auth_tag = encobj.encrypt_and_digest(plaintext)
    except Exception as e:
        return None, Server_error.print_error(Server_error.DATA_ENCRYPTION_ERROR)

    return build_fields_encrypted(auth_tag, ciphertext, encobj.nonce), Server_error.OK

def decrypt(encrypted_data, key):
    error_code = Server_error.OK
    
    auth_tag, nonce, ciphertext, error_code = separare_fields_encrypted(encrypted_data)
    if(error_code != Server_error.OK):
        return None, error_code
        
    try:
        encobj = AES.new(key, AES.MODE_GCM, nonce)
        plaintext = encobj.decrypt_and_verify(ciphertext, auth_tag)
    except Exception as e:
        return None, Server_error.print_error(Server_error.MESSAGE_DECRYPTION_ERROR)

    return plaintext, error_code

def secure_query_db(command, index, id):
    pass

def secure_multi_query_db(command, id):
    pass