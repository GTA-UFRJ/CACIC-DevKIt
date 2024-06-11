# Teleinformatic and Automation Group (GTA, Coppe, UFRJ)
# Author: Guilherme Araujo Thomaz
# Descripton: receive HTTP message
#!/usr/bin/env python

''''''
from http.server import BaseHTTPRequestHandler, HTTPServer
from socketserver import ThreadingMixIn
import threading
import socket


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
        self.send_header('return_status', str(pub_or_query.error))
        self.send_header('return_data', None)
        self.end_headers() 

    def successfull_publication_response(self):
        print_if_debug("Published with success")
        self.send_response(200)
        self.send_header('return_status', 'ack')
        self.send_header('return_data', None)
        self.end_headers() 

    def successfull_query_response(self, query):
        print_if_debug("Queried with success")
        self.send_response(200)
        self.send_header('return_status', 'ack')
        self.send_header('return_data', query.respone)
        self.end_headers() 

    def publish(self):
        try:
            publication = Publication(self.data_for_publish)
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

    def fix_path(self):
        self.path = self.path.replace('%27', "'")

    def do_GET(self):
        self.fix_path()
        print_if_debug("Server received GET ", self.path)

        msg_type = self.path.split('/')[1]

        if(msg_type == "publish"):
            self.data_for_publish = self.path
            self.publish()
        elif(msg_type == "query"):
            self.query()
        else:
            print_if_debug("Unknown GET message type")

    def get_post_body(self):
        content_length = int(self.headers['Content-Length'])
        post_data_bytes = self.rfile.read(content_length)
        return post_data_bytes.decode("utf-8")
    
    def do_POST(self):
        print_if_debug("Server received POST ", self.path)

        msg_type = self.path.split('/')[1]

        if(msg_type == "publish"):
            self.data_for_publish = self.get_post_body()
            self.publish()
        else:
            print_if_debug("Unknown POST message type")

class ThreadingSimpleServer(ThreadingMixIn, HTTPServer):
    """Handle requests in a separate thread."""
    pass


def main():
    print('SCACIC server is starting on IP ', SERVER_IP, ' and port ', SERVER_PORT)
    server_address = (SERVER_IP, SERVER_PORT)
    #server_handler = HTTPServer(server_address, Request_handler)
    server_handler = ThreadingSimpleServer(server_address, Request_handler)
    server_handler.serve_forever()


if __name__ == '__main__':
    print("hello from gramine", flush=True)
    main()
