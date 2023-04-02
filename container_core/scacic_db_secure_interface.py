from scacic_databse_calls import *
from scacic_utils import *

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

    decrypted_fields, error = postprocess_received_query(data, id)
    return decrypted_fields['payload'], error

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