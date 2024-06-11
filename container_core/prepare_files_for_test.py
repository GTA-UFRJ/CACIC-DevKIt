
import secrets
from scacic_utils import * 
import sys

# TODO: encrypt key 

#sys.stdout = open("./container_core/test_id_and_key", 'w')

ca = bytes(16)
print("CA: ", convert_bytes_to_text(ca))
ca_path = './core/server/resources/storage_key_container'
f = open(ca_path, 'wb')
f.write(ca)
f.close()
print("Written in created file: ", ca_path, end='\n\n')

#id = secrets.token_hex(4)
id = "72d41281" 
print("ID: ", id)
client_file_path = './core/server/resources/'+id+"_container"
f = open(client_file_path, 'wb')
print("Created file: ", client_file_path, end='\n\n')

#ck = secrets.token_bytes(16)
ck = b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
print("CK: ", convert_bytes_to_text(ck))

enc_ck, error = encrypt(ck,ca)
if error:
    exit(error)
print("CK encrypted with CA: ", convert_bytes_to_text(enc_ck))

print("Testing decryption")
dec_ck, error = decrypt(enc_ck,ca)
if error:
    exit(error)
if(dec_ck != ck):
    print("Decryption test failed. Decrypted CK: ", dec_ck)
    exit(-1)
print("Decryption successfull")

f.write(enc_ck)
f.close()
print("Written in file: ", client_file_path, end='\n\n')
