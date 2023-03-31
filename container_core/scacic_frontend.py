# Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
# Author: Guilherme Araujo Thomaz
# Descripton: receive HTTP message
#!/usr/bin/env python

from http.server import BaseHTTPRequestHandler, HTTPServer
from scacic_publish import Publication
from scacic_macros import SERVER_IP, SERVER_PORT

class Request_handler(BaseHTTPRequestHandler):

    def finalize_if_failed(self, publication):
        if(publication.succeeded()):
            return
        self.send_response(200)
        self.send_header('return', str(publication.error))
        self.end_headers() 

    def finalize(self):
        self.send_response(200)
        self.send_header('return', 'ack')
        self.end_headers() 

    def do_GET(self):
        print("Received ", self.path)

        publication = Publication(self.path)
        self.finalize_if_failed(publication)

        publication.publication_request_exec()
        self.finalize_if_failed(publication)

        publication.publish_result()
        self.finalize_if_failed(publication)

        self.finalize()

def main():
    print('CACIC server is starting on IP ', SERVER_IP, ' and port ', SERVER_PORT)
    server_address = (SERVER_IP, SERVER_PORT)
    server_handler = HTTPServer(server_address, Request_handler)
    server_handler.serve_forever()

if __name__ == '__main__':
    main()