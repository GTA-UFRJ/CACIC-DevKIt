# Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
# Author: Guilherme Araujo Thomaz
# Descripton: auxiliary functions for encryption, etc

from Crypto.Cipher import AES
from scacic_errors import Server_error
import random
#from scacic_databse_calls import *
#rom scacic_disk_manager import get_ca_key
import secrets
from scacic_macros import *
from datetime import datetime

def get_time():
    current_datetime = datetime.now()
    desired_format = "%Y-%m-%d.%H:%M:%S"
    formatted_time = current_datetime.strftime(desired_format)
    return formatted_time

def print_if_debug(*args, sep=''):
    if DEBUG:
        result = ""
        for arg in args:
            result = result + str(arg) + sep
        print(result)

def convert_text_to_bytes(text):
    return bytes.fromhex(text.replace('-',''))

def convert_bytes_to_text(byte):
    hex_str = byte.hex()
    return '-'.join(hex_str[i:i+2] for i in range(0, len(hex_str)+1, 2))

def get_random_id():
    digits = '0123456789'
    return ''.join(random.choices(digits, k=8))

def get_random_nonce():
    return secrets.token_bytes(12)

def separare_fields_encrypted(encrypted_data):
    if(len(encrypted_data) <= 16+12):
        return None, None, None, Server_error.print_error(Server_error.INVALID_ENCRYPTED_FIELD_ERROR)
    mac, iv, data = encrypted_data[:16], encrypted_data[16:16+12], encrypted_data[16+12:]
    return mac, iv, data, Server_error.OK

def build_fields_encrypted(mac, data, nonce):
    return mac + nonce + data

# TODO: mudar para AES-CTR
def encrypt(input, key):
    try:    
        encobj = AES.new(key, AES.MODE_GCM, nonce=get_random_nonce())
        ciphertext, auth_tag = encobj.encrypt_and_digest(input)
    except Exception as e:
        print("Encryption error: ", e)
        return None, Server_error.print_error(Server_error.DATA_ENCRYPTION_ERROR)

    result_bytes = build_fields_encrypted(auth_tag, ciphertext, encobj.nonce)

    return result_bytes, Server_error.OK

def decrypt(encrypted_data, key):
    auth_tag, nonce, ciphertext, error_code = separare_fields_encrypted(encrypted_data)
    if(error_code != Server_error.OK):
        return None, error_code
        
    try:
        encobj = AES.new(key, AES.MODE_GCM, nonce)
        plaintext = encobj.decrypt_and_verify(ciphertext, auth_tag)
    except Exception as e:
        print("Decryption error: ", e)
        return None, Server_error.print_error(Server_error.MESSAGE_DECRYPTION_ERROR)

    return plaintext, Server_error.OK

# time|...|pk|...|type|...|payload|...|permission1|...|permission2|...
# TODO: fw_id=...&pk=...&vn=...&payload=...&permissions={}
def parse_fields_decrypted(decrypted):
    fields_list = decrypted.split('|')
    if(len(fields_list) < 10):
        return None, Server_error.print_error(Server_error.EMPTY_PERMISSIONS_ERROR)

    time, id, type, payload  = [fields_list[index] for index in range(1,8,2)]
    permissions_list = [fields_list[index] for index in range(9,len(fields_list)) if index % 2 != 0]
    return {'time':time,'id':id,'type':type,'payload':payload,'perms_list':permissions_list}, Server_error.OK

def authenticate_client(received_id, decrypted_id):
    return received_id == decrypted_id   

def verify_permissions(perms_list, id):
    return id in perms_list

#print(convert_bytes_to_text(convert_text_to_bytes("a5-23-12-f1-ab-")))

if __name__ == '__main__':
    key = b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
    plaintext = 'teste'
    print("Plaintext: ", plaintext, '(%d)'%len(plaintext))
    encrypted, error = encrypt(plaintext.encode(), key)
    if error != Server_error.OK:
        exit()
    print("Encrypted: ", convert_bytes_to_text(encrypted), '(%d)'%(len(convert_bytes_to_text(encrypted))/3))
    decrypted, error = decrypt(encrypted, key)
    if error != Server_error.OK:
        exit()
    print("Decrypted: ", decrypted.decode(), '(%d)'%len(plaintext))
