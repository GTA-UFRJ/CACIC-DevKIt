#!/usr/bin/env python

from http.server import BaseHTTPRequestHandler, HTTPServer

server_ip = '127.0.0.1'
server_port = 8080

class Request_handler(BaseHTTPRequestHandler):

    def parse_fields(self):
        fields_list = self.path.split('/')[3].split('|')
        fields_dict = {
            'pk' : fields_list[1],
            'type' : fields_list[3],
            'size' : fields_list[5],
            'encrypted' : fields_list[7]
        }
        return fields_dict

    def task_1(self):
        self.send_header('return','Vou treinar')

    def task_2(self):
        self.send_header('return','Vou inferir')

    def execute_task(self):

        server_functions = {
            '222222' : self.task_1,
            '333333' : self.task_2
        }

        fields = self.parse_fields()
        
        if(fields['type'] not in server_functions.keys()):
            self.send_header('return', 'Falhou')

        server_functions[fields['type']]()

    def do_GET(self):
        print("Received ", self.path)    
        self.send_response(200)
        self.execute_task()
        self.end_headers()   

print('http server is starting on IP ', server_ip, ' and port ', server_port)
server_address = (server_ip, server_port)
server_handler = HTTPServer(server_address, Request_handler)
print('http server is running...')
server_handler.serve_forever()
