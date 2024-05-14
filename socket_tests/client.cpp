#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // !Create a socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    // !Server address
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080); // !Connect to port 8080
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); // !Connect to localhost

    // !Connect to the server
    if (connect(client_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
        std::cerr << "Connection failed\n";
        return 1;
    }

    // !Receive HTML content from server
    char buffer[1024] = {0};
    recv(client_fd, buffer, 1024, 0);
    std::cout << "HTML content received from server:\n" << buffer << std::endl;

    // !Close the socket
    close(client_fd);

    return 0;
}


