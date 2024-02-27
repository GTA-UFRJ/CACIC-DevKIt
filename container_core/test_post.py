import http.client as httplib

server_ip = '127.0.0.1'
server_port = 8080

connection = httplib.HTTPConnection(server_ip, server_port)

try:
    connection.request('POST', '/publish', "/publish/size=5/teste")
    response = connection.getresponse()

    if(response.status != 200):
        print("Error ", response.status, ": ", response.reason)
    else:
        header_received = response.getheader('return_status')
        print(header_received)
except:
    print("Could not stabilish/mantain connection")
