# Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
# Author: Guilherme Araujo Thomaz
# Descripton: auxiliary functions for encryption, etc

from Crypto.Cipher import AES
from Crypto.Hash import SHA256
from Crypto.Util import Counter
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
        print(result, flush=True)

def convert_text_to_bytes(text):
    return bytes.fromhex(text.replace('-',''))

def convert_bytes_to_text(byte):
    hex_str = byte.hex()
    return '-'.join(hex_str[i:i+2] for i in range(0, len(hex_str)+1, 2))

def get_random_id():
    digits = '0123456789'
    return ''.join(random.choices(digits, k=8))

def get_random_nonce():
    return secrets.token_bytes(IV_SIZE)

def separare_fields_encrypted(encrypted_data):
    if(len(encrypted_data) <= MAC_SIZE+IV_SIZE):
        return None, None, None, Server_error.print_error(Server_error.INVALID_ENCRYPTED_FIELD_ERROR)
    mac, iv, data = encrypted_data[:MAC_SIZE], encrypted_data[MAC_SIZE:MAC_SIZE+IV_SIZE], encrypted_data[MAC_SIZE+IV_SIZE:]
    return mac, iv, data, Server_error.OK

def build_fields_encrypted(mac, data, nonce):
    return mac + nonce + data

# TODO: mudar para AES-CTR
def encrypt(input, key):
    try:    
        nonce = get_random_nonce()
        counter = Counter.new(128, initial_value=int(nonce.hex(),16))
        encobj = AES.new(key, AES.MODE_CTR, counter=counter)
        ciphertext = b''
        for i in range(0,len(input),32):
            block = input[i:i+32]
            ciphertext += encobj.encrypt(block)
        hash_object = SHA256.new(data=input)
        hash_object.update(nonce)
        #hash_object.update(key)
        auth_tag = hash_object.digest()
    except Exception as e:
        print("Encryption error: ", e)
        return None, Server_error.print_error(Server_error.DATA_ENCRYPTION_ERROR)

    result_bytes = build_fields_encrypted(auth_tag, ciphertext, nonce)
    
    #print(convert_bytes_to_text(input))
    #print(convert_bytes_to_text(nonce))
    #print(convert_bytes_to_text(ciphertext))
    #print(convert_bytes_to_text(auth_tag))
    #print(convert_bytes_to_text(result_bytes))

    return result_bytes, Server_error.OK

def decrypt(encrypted_data, key):
    auth_tag, nonce, ciphertext, error_code = separare_fields_encrypted(encrypted_data)
    if(error_code != Server_error.OK):
        return None, error_code
        
    try:
        counter = Counter.new(128, initial_value=int(nonce.hex(),16))
        encobj = AES.new(key, AES.MODE_CTR, counter=counter)
        plaintext = b''
        for i in range(0,len(ciphertext),32):
            block = ciphertext[i:i+32]
            plaintext += encobj.decrypt(block)
        hash_object = SHA256.new(data=plaintext)
        hash_object.update(nonce)
        #hash_object.update(key)
        retrieved_mac_tag = hash_object.digest()
    except Exception as e:
        print("Decryption error: ", e)
        return None, Server_error.print_error(Server_error.MESSAGE_DECRYPTION_ERROR)
    
    #print(convert_bytes_to_text(plaintext))
    #print(convert_bytes_to_text(nonce))
    #print(convert_bytes_to_text(ciphertext))
    #print(convert_bytes_to_text(retrieved_mac_tag))
    #print(convert_bytes_to_text(encrypted_data))

    if(retrieved_mac_tag != auth_tag):
        return None, Server_error.print_error(Server_error.MESSAGE_DECRYPTION_ERROR)
    
    return plaintext, Server_error.OK

# time|...|pk|...|type|...|fw|...|vn|...|payload|...|permission1|...|permission2|...
# TODO: fw_id=...&pk=...&vn=...&payload=...&permissions={}
def parse_fields_decrypted(decrypted):
    fields_list = decrypted.split('|')
    if(len(fields_list) < 10):
        return None, Server_error.print_error(Server_error.EMPTY_PERMISSIONS_ERROR)

    time, id, type, fw, vn, payload  = [fields_list[index] for index in range(1,12,2)]
    permissions_list = [fields_list[index] for index in range(13,len(fields_list)) if index % 2 != 0]
    return {'time':time,'id':id,'type':type,'fw':fw,'vn':vn,'payload':payload,'perms_list':permissions_list}, Server_error.OK

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
