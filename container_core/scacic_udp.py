import socket
import time
from scacic_macros import *
from scacic_query import Query

class UDPServer:
    def __init__(self, ip="0.0.0.0", port=8080, buffer_size=1024, total_packets=200):
        self.udp_ip = ip
        self.udp_port = port
        self.buffer_size = buffer_size
        self.total_packets = total_packets
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind((self.udp_ip, self.udp_port))
        print(f"Server listening on port {self.udp_port}")

    def rebuild_message_from_fields(self, fields):
        message = ""
        for field in fields:
            message = message + field + "|"
        return message[:-1]

    def listen(self):
        while True:
            request, addr = self.sock.recvfrom(self.buffer_size)
            print(f"Message received from {addr}")
            self.send_packets(addr, request)

    def modify_query_response(self, response, index):
        # size|%02x|index|%02x|encrypted|...
        
        response_fields = response.split('|')
        response_fields.insert(2, "index")
        response_fields.insert(3, index)
        return self.rebuild_message_from_fields(response_fields)

    def query(self, query_message):
        try:
            print(query_message)
            query = Query(query_message)
            query.query_request_exec()
            return query.respone
        except Exception as e:
            print(e)
            return None

    def send_packets(self, addr, request):

        # Exemplo: pk|72d41281|index|1,2,3,4|size|23|command|SELECT_*_from_TACIOT_where_type=%%27123456%%27_and_fw=%%27%u%%27_and_vn=%%27%u%%27|encrypted|

        query_message_fields = request.decode().split('|')
        missing_packets_indexes = query_message_fields[3].split(',')[:-1]
        print(f"Requested packets: {missing_packets_indexes}")

        for packet_index in missing_packets_indexes:
            query_message_fields[3] = packet_index

            # FOR DEBUG:
            query_message_fields[3] = "3"

            individual_query = self.rebuild_message_from_fields(query_message_fields)
            response = self.query("///"+individual_query.replace('%27', "'"))
            if response is not None:
                response = self.modify_query_response(response, packet_index)
                self.sock.sendto(response.encode(), addr)

if __name__ == "__main__":
    server = UDPServer(ip=SERVER_IP, port=SERVER_PORT)
    server.listen()
