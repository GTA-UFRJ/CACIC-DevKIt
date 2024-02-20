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

    def error_response(self, pub_or_query):
        self.send_response(200)
        if(pub_or_query.error == Server_error.OK):
            pub_or_query.error = -1
        self.send_header('return', str(pub_or_query.error))
        self.end_headers() 

    def successfull_publication_response(self):
        print_if_debug("Published with success")
        self.send_response(200)
        self.send_header('return', 'ack')
        self.end_headers() 

    def successfull_query_response(self, query):
        print_if_debug("Queried with success")
        self.send_response(200)
        self.send_header('return', query.respone)
        self.end_headers() 

    def publish(self):
        try:
            publication = Publication(self.path)
            publication.publication_request_exec()
            publication.publish_result()
            self.successfull_publication_response()

        except Exception:
            self.error_response(publication)

    def query(self):
        try:
            query = Query(self.path)
            query.query_request_exec()
            self.successfull_query_response(query)

        except Exception:
            self.error_response(query)

    def do_GET(self):
        print_if_debug("Server received ", self.path)

        msg_type = self.path.split('/')[1]

        if(msg_type == "publish"):
            self.publish()
        elif(msg_type == "query"):
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