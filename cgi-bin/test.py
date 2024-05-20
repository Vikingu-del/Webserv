from socket import *

def http_req(server, path):

	# Creating a socket to connect and read from
	s=socket(AF_INET,SOCK_STREAM)

	# Finding server address (assuming port 80)
	adr=(gethostbyname(server),80)

	# Connecting to server
	s.connect(adr)

	# Sending request
	request = "GET "+path+" HTTP/1.0\r\n\r\n"
	s.send(request.encode())

	# Printing response
	resp=s.recv(1024)
	while resp!="":
		print(resp)
		resp=s.recv(1024)

# Test the function
http_req("birk105.studby.uio.no","/")
		