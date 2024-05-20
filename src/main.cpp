/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 15:15:34 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/20 09:00:04 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

void	testHttpHandler();

int main(int argc, char **argv)
{

	std::string configFilePath;
	ConfigFileParser parser;

	try {
		switch (argc) 
		{
			case 1:
				configFilePath = "configs/siege.conf";
				// parser.parseConfigFile(configFilePath);
				// parser.printServers();
				// After parsing setup all needed info
				// Run the servers
				break;
			case 2:
				configFilePath = argv[1];
				// parser.parseConfigFile(configFilePath);
				// parser.printServers();
				// After parsing setup all needed info
				// Run the servers
				break;
			default:
				throw std::invalid_argument("Wrong number arguments");
				return 1;
		}
		testHttpHandler();
	}
	catch (std::exception & ex) {
		std::cerr << ex.what() << std::endl;
		return 1;
	}
	return 0;
}

// #include <sys/socket.h>
// #include <netdb.h>
// #include <unistd.h>
// #include <fstream>
// #include <cstring>

// int main(int argc, char *argv[])
// {
//     if (argc != 4) {
//         std::cout << "Usage: ./http.elf [HOST-NAME] [RESOURCE] [OUTPUT-FILE]" << std::endl;
//         return -1;
//     }
//     std::string host (argv[1]);
//     std::string resource (argv[2]);
//     std::string outputFile (argv[3]);
    
//     HTTP::Header hostHdr = HTTP::Header("Host", host);
//     HTTP::Header dntHdr = HTTP::Header("DNT", "1");

// 	HTTP::Header connectionHdr = HTTP::Header("Connection", "close");
    
//     std::map<std::string, HTTP::Header> headers;
//     headers.insert(std::make_pair(hostHdr.getKey(), hostHdr));
//     headers.insert(std::make_pair(dntHdr.getKey(), dntHdr));
// 	headers.insert(std::make_pair(connectionHdr.getKey(), connectionHdr));
    
//     HTTP::Request request(HTTP::Method::GET, resource, headers, HTTP::Version::HTTP_1_1);
//     std::string httpRequest = request.serialize();

//     // Create a socket
//     int socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
//     if (socketFileDescriptor < 0) {
//         std::cerr << "Error: Could not create socket" << std::endl;
//         return -1;
//     }

//     // Get server details
//     struct hostent *server = gethostbyname(host.c_str());
//     if (server == NULL) {
//         std::cerr << "Error: Could not find host" << std::endl;
//         return -1;
//     }

//     // Connect to the server
//     struct sockaddr_in serverAddress;
//     bzero((char *) &serverAddress, sizeof(serverAddress));
//     serverAddress.sin_family = AF_INET;
//     bcopy((char *)server->h_addr, (char *)&serverAddress.sin_addr.s_addr, server->h_length);
//     serverAddress.sin_port = htons(80); // HTTP uses port 80

//     if (connect(socketFileDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
//         std::cerr << "Error: Could not connect to host" << std::endl;
//         return -1;
//     }

//     // Send the HTTP request
//     if (write(socketFileDescriptor, httpRequest.c_str(), httpRequest.length()) < 0) {
//         std::cerr << "Error: Could not send request" << std::endl;
//         return -1;
//     }

//     // Receive the response
// 	char buffer[256];
// 	std::string response;
// 	int bytesRead;
// 	while ((bytesRead = read(socketFileDescriptor, buffer, 255)) > 0) {
// 		std::cout << "Read " << bytesRead << " bytes from server" << std::endl;
// 		 response.append(buffer, bytesRead);
//    		 bzero(buffer, 256);
// 	}

// 	if (bytesRead < 0) {
// 		std::cerr << "Error: Could not read response" << std::endl;
// 		return -1;
// 	}

//     // Close the socket
//     close(socketFileDescriptor);

//     return 0;
// }