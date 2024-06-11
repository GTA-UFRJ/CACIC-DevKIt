#!/usr/bin/env python

import http.client as httplib
from scacic_utils import *
from scacic_disk_manager import *

server_ip = '146.164.69.148'
server_port = 8080

connection = httplib.HTTPConnection(server_ip, server_port)

f = open('./core/server/resources/storage_key_container','rb')
ca = f.read()
f.close()

id = '72d41281'
f = open('./core/server/resources/'+id+'_container','rb')
enc_ck = f.read()
ck, _ = decrypt(enc_ck, ca)
print("ID: ", id)
print("CK: ", convert_bytes_to_text(ck))

# PUBLISH WITH GET

#payload = "time|10h30m47s|pk|72d41281|type|123456|fw|654321|vn|789101|payload|aaaa|permission1|" + id
#payload = "time|10h30m47s|pk|72d41281|type|123456|fw|654321|vn|789101|payload|aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa|permission1|72d41281"
payload = "time|2024-03-04.15:00:48|pk|72d41281|type|123456|fw|789101|vn|654321|payload|aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa|permissions0|72d41281"

enc, _ = encrypt(payload.encode(), ck)
time = get_time()
enc_text = convert_bytes_to_text(enc)
request = 'time|' + time + '|pk|' + id + '|type|123456|fw|789101|vn|654321|size|' + str(len(enc_text)) + '|encrypted|' + enc_text
print(request)

publish_complete_request = '/publish/size=' + str(len(request)) + '/' + request

try:
    connection.request('GET', publish_complete_request)
    response = connection.getresponse()

    if(response.status != 200):
        print("Error ", response.status, ": ", response.reason)
    else:
        header_received = response.getheader('return_status')
        print(header_received)
except:
    print("Could not stabilish/mantain connection")

key = ''
key = input("Type any key to test query. Type 'q' to exit.")
if(key == 'q' or key == "Q"):
    exit(0)

# QUERY

# pk|72d41281|index|000000|size|23|command|SELECT_*_FROM_TACIOT_WHERE_TYPE='555555'|encrypted|
index = 12
command = "SELECT_*_FROM_TACIOT"
enc, _ = encrypt(id.encode(), ck)
enc_text = convert_bytes_to_text(enc)
request = "pk|" + id + "|index|" + str(index).zfill(6) + "|size|" + format(len(command),'02x') + "|command|" + command + "|encrypted|" + enc_text

print(request)

complete_request = '/query/size=' + str(len(request)) + '/' + request

try:
    connection.request('GET', complete_request)
    response = connection.getresponse()

    if(response.status != 200):
        print("Error ", response.status, ": ", response.reason)
    else:

        header_received = response.getheader('return_status')
        if(header_received != 'ack'):
            print(header_received)
        else:
            header_received = response.getheader('return_data')
            bytes = convert_text_to_bytes(header_received.split('|')[3])
            data,_ = decrypt(bytes, ck)
            print(data.decode())
except:
    print("Could not stabilish/mantain connection")


key = ''
key = input("Type any key to test publish with POST. Type 'q' to exit.")
if(key == 'q' or key == "Q"):
    exit(0)

# PUBLISH WITH POST

try:
    connection.request('POST', '/publish', publish_complete_request)
    response = connection.getresponse()

    if(response.status != 200):
        print("Error ", response.status, ": ", response.reason)
    else:
        header_received = response.getheader('return_status')
        print(header_received)
except:
    print("Could not stabilish/mantain connection")

connection.close()