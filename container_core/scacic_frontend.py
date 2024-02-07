# Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
# Author: Guilherme Araujo Thomaz
# Descripton: receive HTTP message
#!/usr/bin/env python

from http.server import BaseHTTPRequestHandler, HTTPServer
from scacic_publish import Publication
from scacic_query import Query
from scacic_macros import SERVER_IP, SERVER_PORT
from scacic_utils import * 

# TODO: colocar multithreaded
class Request_handler(BaseHTTPRequestHandler):

    def finalize_if_failed(self, message_polymorphic_object):
        if(message_polymorphic_object.succeeded()):
            return
        self.send_response(200)
        self.send_header('return', str(message_polymorphic_object.error))
        self.end_headers() 

    def publish(self):
        try:
            publication = Publication(self.path)
            publication.publication_request_exec()
            publication.publish_result()

            print_if_debug("Published with success")
            self.send_response(200)
            self.send_header('return', 'ack')
            self.end_headers() 

        except Exception:
            self.finalize_if_failed(publication)

    def query(self):
        try:
            query = Query(self.path)
            query.query_request_exec()

            print_if_debug("Queried with success")
            self.send_response(200)
            self.send_header('return', query.respone)
            self.end_headers() 

        except Exception:
            self.finalize_if_failed(query)

    def do_GET(self):
        print_if_debug("Server received ", self.path)

        msg_type = self.path.split('/')[1]

        if(msg_type == "Publication"):
            self.publish()
        elif(msg_type == "Query"):
            self.query()
        else:
            print_if_debug("Unknown message type")

def main():
    print('SCACIC server is starting on IP ', SERVER_IP, ' and port ', SERVER_PORT)
    server_address = (SERVER_IP, SERVER_PORT)
    server_handler = HTTPServer(server_address, Request_handler)
    server_handler.serve_forever()

if __name__ == '__main__':
    main()