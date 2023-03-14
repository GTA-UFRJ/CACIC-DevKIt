#!/usr/bin/env python

import http.client as httplib

server_ip = '127.0.0.1'
server_port = 8080

connection = httplib.HTTPConnection(server_ip, server_port)

connection.request('GET', '/publish/size=51/pk|72d41281|type|222222|size|03|encrypted|dd-b1-b6-')

response = connection.getresponse()

if(response.status != 200):
    print("Error ", response.status, ": ", response.reason)
else:
    header_received = response.getheader('return')
    print(header_received)

connection.close()