#!/usr/bin/env python

import http.client as httplib
from scacic_utils import *
from scacic_disk_manager import *

server_ip = '127.0.0.1'
server_port = 8080

connection = httplib.HTTPConnection(server_ip, server_port)

bytes, _ = encrypt('arquivo_teste', get_ca_key())
text = convert_bytes_to_text(bytes)
request = 'time|xxxxxxxxxxxxxxxxxxx|pk|72d41281|type|222222|size|' + len(text) + '|encrypted|' + text
print(request)

connection.request('GET', '/publish/size=%d/%s', len(request), request)

response = connection.getresponse()

if(response.status != 200):
    print("Error ", response.status, ": ", response.reason)
else:
    header_received = response.getheader('return')
    print(header_received)

connection.close()