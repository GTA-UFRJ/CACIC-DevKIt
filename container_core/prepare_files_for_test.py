
import secrets
from scacic_utils import * 
import sys

sys.stdout = open("./container_core/test_id_and_key", 'w')

ca = bytes(16)
f = open('./core/server/resources/storage_key_container', 'wb')
f.write(ca)
f.close()

id = secrets.token_hex(4)
print("id: ", id)
ck = secrets.token_bytes(16)
print("ck: ", convert_bytes_to_text(ck))
f = open('./core/server/resources/'+id+"_container", 'wb')
f.write(ck)
f.close()
