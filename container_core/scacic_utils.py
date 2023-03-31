# Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
# Author: Guilherme Araujo Thomaz
# Descripton: auxiliary functions for encryption, etc

from Crypto.Cipher import AES
from scacic_errors import Server_error
import random
from scacic_databse_calls import db_query, db_multi_query
from scacic_disk_manager import get_ca_key

def convert_text_to_bytes(text):
    return bytes.fromhex(text.replace('-',''))

def convert_bytes_to_text(byte):
    hex_str = byte.hex()
    return '-'.join(hex_str[i:i+2] for i in range(0, len(hex_str)+1, 2))

def get_random_id():
    digits = '0123456789'
    return ''.join(random.choices(digits, k=8))

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

def decrypt(encrypted_data, key, return_format='bytes'):
    auth_tag, nonce, ciphertext, error_code = separare_fields_encrypted(encrypted_data)
    if(error_code != Server_error.OK):
        return None, error_code
        
    try:
        encobj = AES.new(key, AES.MODE_GCM, nonce)
        plaintext = encobj.decrypt_and_verify(ciphertext, auth_tag)
    except Exception as e:
        return None, Server_error.print_error(Server_error.MESSAGE_DECRYPTION_ERROR)

    if(return_format == 'text'):
        return plaintext.decode('ascii'), Server_error.OK 

    return plaintext, Server_error.OK

# time|...|pk|...|type|...|payload|...|permission1|...|permission2|...
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

def postprocess_received_query(data, id):
    ca = get_ca_key()
    decrypted, error = decrypt(data['encrypted'], ca, return_format='text')
    if(error != Server_error.OK):
        return None, error

    decrypted_fields, error = parse_fields_decrypted(decrypted)
    if(error != Server_error.OK):
        return None, error
    
    if(authenticate_client(data['pk'], decrypted_fields['id'])):
        return None, Server_error.print_error(Server_error.DATA_VALIDITY_ERROR)
    
    if(verify_permissions(decrypted_fields['perms_list'],id)):
        return None, Server_error.print_error(Server_error.ACCESS_DENIED)
    
    return decrypted_fields, Server_error.OK

def secure_query_db(command, index, id):
    data, error = db_query(command, index)
    if(error != Server_error.OK):
        return None, error

    data['decrypted_fields'], error = postprocess_received_query(data, id)
    return data, error

def secure_multi_query_db(command, id):
    data_list, error = db_multi_query(command)
    if(error != Server_error.OK):
        return None, error

    data_list = []
    for data in data_list:
        data['decrypted_fields'], error = postprocess_received_query(data, id)
        if(error == Server_error.OK):
            data_list.append(data)

    return data_list, Server_error.OK

#print(convert_bytes_to_text(convert_text_to_bytes("a5-23-12-f1-ab-")))