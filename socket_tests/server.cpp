#include <iostream>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// int main() {
//     // Create a socket
//     int server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd == -1) {
//         std::cerr << "Socket creation failed\n";
//         return 1;
//     }

//     // Bind the socket to a port
//     sockaddr_in server_addr;
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP
//     server_addr.sin_port = htons(8080); // Listen on port 8080

//     if (bind(server_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
//         std::cerr << "Bind failed\n";
//         return 1;
//     }

//     // Listen for incoming connections
//     if (listen(server_fd, 5) == -1) {
//         std::cerr << "Listen failed\n";
//         return 1;
//     }
// // while (true) {
//         // Accept incoming connections in a loop
//         std::cout << "Waiting for incoming connection...\n";

//         // Accept incoming connection
//         int client_socket = accept(server_fd, nullptr, nullptr);
//         if (client_socket == -1) {
//             std::cerr << "Accept failed\n";
//             return 0;
//             // continue;
//         }

//         // Read index.html file
//         std::ifstream file("index.html");
//         std::string html_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//         file.close();

//         // Send HTML content to client
//         send(client_socket, html_content.c_str(), html_content.size(), 0);
//         std::cout << "HTML content sent to client\n";

//         // Close the client socket (not necessary if you want to keep the server running)
//         close(client_socket);
//     // }

//     close(server_fd);

//     return 0;
// }


#include <iostream>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    // Create a socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    // Bind the socket to a port
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP
    server_addr.sin_port = htons(8080); // Listen on port 8080

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
        std::cerr << "Bind failed\n";
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) == -1) {
        std::cerr << "Listen failed\n";
        return 1;
    }

    while (true) {
        // Accept incoming connection
        std::cout << "Waiting for incoming connection...\n";
        int client_socket = accept(server_fd, nullptr, nullptr);
        if (client_socket == -1) {
            std::cerr << "Accept failed\n";
            continue;
        }

        // Read index.html file
        std::ifstream file("index.html");
        std::string html_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        // Send HTTP response with HTML content type
        std::string http_response = "HTTP/1.1 200 OK\r\n";
        http_response += "Content-Type: text/html\r\n";
        http_response += "\r\n"; // Empty line indicating end of headers
        http_response += html_content;

        // Send HTTP response to client
        send(client_socket, http_response.c_str(), http_response.size(), 0);
        std::cout << "HTML content sent to client\n";

        // Close the client socket
        close(client_socket);
    }

    // Close the server socket (unreachable in this example)
    close(server_fd);

    return 0;
}