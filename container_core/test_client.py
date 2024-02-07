#!/usr/bin/env python

import http.client as httplib
from scacic_utils import *
from scacic_disk_manager import *

server_ip = '127.0.0.1'
server_port = 8080

connection = httplib.HTTPConnection(server_ip, server_port)

f = open('./core/server/resources/storage_key_container','rb')
ca = f.read()
f.close()

id = 'dea73a76'
f = open('./core/server/resources/'+id+'_container','rb')
enc_ck = f.read()
ck, _ = decrypt(enc_ck, ca)
print("ID: ", id)
print("CK: ", convert_bytes_to_text(ck))

payload = "time|10h30m47s|pk|72d41281|type|123456|payload|250|permission1|72d41281"

enc, _ = encrypt(payload.encode(), ck)
time = get_time()
enc_text = convert_bytes_to_text(enc)
request = 'time|' + time + '|pk|' + id + '|type|555555|size|' + str(len(enc_text)) + '|encrypted|' + enc_text
print(request)

complete_request = '/publish/size=' + str(len(request)) + '/' + request

try:
    connection.request('GET', complete_request)
    response = connection.getresponse()

    if(response.status != 200):
        print("Error ", response.status, ": ", response.reason)
    else:
        header_received = response.getheader('return')
        print(header_received)
except:
    print("Could not stabilish/mantain connection")

key = input("Type any key to test query. Type 'q' to exit.")
if(key == 'q' or key == "Q"):
    exit(0)

connection.close()