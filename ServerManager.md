ServerSocket and Client
This section details the ServerSocket and Client classes used to manage server connections and client interactions in our web server.

Client Class
The Client class manages individual client connections, handling requests and responses, and maintaining client-specific state information.

Constructor & Destructor
Client::Client(): Initializes the client with the current time.
Client::~Client(): Default destructor.
Copy Constructor & Assignment Operator
Client::Client(const Client &other): Creates a copy of another Client object.
Client &Client::operator=(const Client &rhs): Assigns values from another Client object to the current one.
Member Functions
Client::Client(ServerConf &server): Initializes the client with a given server configuration.
void Client::setSocket(int &sock): Sets the client's socket.
void Client::setAddress(sockaddr_in &addr): Sets the client's address.
void Client::setServer(ServerConf &server): Configures the server for the client.
const int &Client::getSocket() const: Returns the client's socket.
const Request &Client::getRequest() const: Returns the client's request.
const struct sockaddr_in &Client::getAddress() const: Returns the client's address.
const time_t &Client::getLastTime() const: Returns the last message time of the client.
void Client::buildResponse(): Builds the response based on the request.
void Client::updateTime(): Updates the last message time.
void Client::clearClient(): Clears the client's request and response.
ServerSocket Class
The ServerSocket class is responsible for managing multiple server instances, handling client connections, and processing requests and responses.

Constructor & Destructor
ServerSocket::ServerSocket(): Initializes a new ServerSocket instance.
ServerSocket::~ServerSocket(): Default destructor.
Member Functions
void ServerSocket::setupServers(std::vector<ServerConf> servers): Initializes all servers based on the configuration provided.
void ServerSocket::runServers(): Main loop for managing file descriptors, accepting new clients, and processing requests.
void ServerSocket::checkTimeout(): Checks if clients have timed out and closes connections if necessary.
void ServerSocket::initializeSets(): Initializes file descriptor sets and adds server sockets.
void ServerSocket::acceptNewConnection(ServerConf &serv): Accepts a new client connection and adds it to the client map.
void ServerSocket::closeConnection(const int i): Closes a connection and removes the associated client from the map.
void ServerSocket::sendResponse(const int &i, Client &c): Sends a response to the client and handles connection persistence.
void ServerSocket::assignServer(Client &c): Assigns the appropriate server configuration to a client based on the request.
void ServerSocket::readRequest(const int &i, Client &c): Reads a request from the client and processes it.
void ServerSocket::handleReqBody(Client &c): Handles the request body for CGI scripts.
void ServerSocket::sendCgiBody(Client &c, Cgi &cgi): Sends the request body to a CGI script.
void ServerSocket::readCgiResponse(Client &c, Cgi &cgi): Reads the response from a CGI script.
void ServerSocket::addToSet(const int i, fd_set &set): Adds a file descriptor to a set.
void ServerSocket::removeFromSet(const int i, fd_set &set): Removes a file descriptor from a set.
Example Usage
Here’s a brief example of how these classes are used together:

cpp
Copy code
#include "ServerSocket.hpp"
#include "Client.hpp"

// Initialize server configurations
std::vector<ServerConf> serverConfigs;
// Populate serverConfigs with ServerConf instances

ServerSocket serverSocket;
serverSocket.setupServers(serverConfigs);
serverSocket.runServers();
This example sets up multiple servers and starts the main server loop.