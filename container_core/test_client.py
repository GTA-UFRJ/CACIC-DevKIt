#!/usr/bin/env python

import http.client as httplib
from scacic_utils import *
from scacic_disk_manager import *

server_ip = '127.0.0.1'
server_port = 8080

connection = httplib.HTTPConnection(server_ip, server_port)

f = open("./container_core/test_id_and_key", 'r')
lines = f.readlines()
id = lines[0].split(' ')[1][:-1]
print("id: ",id)
ck_text = lines[1].split(' ')[1][:-1]
print("ck: ",ck_text)
ck = convert_text_to_bytes(ck_text)

enc_text, _ = encrypt('testing', ck, return_format='text')
request = 'time|xxxxxxxxxxxxxxxxxxx|pk|' + id + '|type|222222|size|' + str(len(enc_text)) + '|encrypted|' + enc_text
print(request)

complete_request = '/publish/size=' + str(len(request)) + '/' + request

connection.request('GET', complete_request)

response = connection.getresponse()

if(response.status != 200):
    print("Error ", response.status, ": ", response.reason)
else:
    header_received = response.getheader('return')
    print(header_received)

connection.close()