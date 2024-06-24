#include "ServerSocket.hpp"

int setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        // Logger::logMsg(RED, CONSOLE_OUTPUT, "fcntl F_GETFL error: %s", strerror(errno));
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        // Logger::logMsg(RED, CONSOLE_OUTPUT, "fcntl F_SETFL error: %s", strerror(errno));
        return -1;
    }
    return 0;
}

ServerSocket::ServerSocket() {
    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd == -1) {
        // Logger::logMsg(RED, CONSOLE_OUTPUT, "epoll_create1 error: %s", strerror(errno));
        exit(1);
    }
}

ServerSocket::~ServerSocket() {
    for (std::map<int, Client>::iterator it = _clientsMap.begin(); it != _clientsMap.end(); ++it) {
        close(it->first);
    }
    _clientsMap.clear();
    close(epoll_fd);
}

void ServerSocket::addToEpoll(const int fd, uint32_t events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "Failed to add fd %d to epoll instance with events %u: %s", fd, events, strerror(errno));
        close(fd);
        return; // Do not close epoll_fd here, it is for the entire server
    }
    Logger::logMsg(GREEN, CONSOLE_OUTPUT, "Added fd %d to epoll with events %u", fd, events);
}

void ServerSocket::modifyEpoll(int fd, uint32_t events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "epoll_ctl EPOLL_CTL_MOD error: %s", strerror(errno));
        exit(1);
    }
}

void ServerSocket::removeFromEpoll(int fd) {
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, 0) == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "epoll_ctl EPOLL_CTL_DEL error: %s", strerror(errno));
    }
    close(fd);
}

// Adapted acceptNewConnection function for epoll
void ServerSocket::acceptNewConnection(ServerConfig& serverConfig) {
    struct sockaddr_in clientAddress;
    socklen_t clientAddress_size = sizeof(clientAddress);
    int clientSocket = accept(serverConfig.getListenFd(), (struct sockaddr *)&clientAddress, &clientAddress_size);
    if (clientSocket == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "Failed to accept new connection: %s", strerror(errno));
        return;
    }
    
    Client newClient(serverConfig);
    char buf[INET_ADDRSTRLEN];
    Logger::logMsg(YELLOW, CONSOLE_OUTPUT, "New Connection From %s, Assigned Socket %d",
                   inet_ntop(AF_INET, &clientAddress, buf, INET_ADDRSTRLEN), clientSocket);
    
    if (setNonBlocking(clientSocket) == -1) {
        close(clientSocket);
        return;
    }
    
    // Add the new client to the epoll instance for monitoring
    addToEpoll(clientSocket, EPOLLIN);

    // Create a new Client object and store it in the _clientsMap
    if (_clientsMap.count(clientSocket) == 0)
        _clientsMap.erase(clientSocket);
    _clientsMap.insert(std::make_pair(clientSocket, newClient));
}


void ServerSocket::listenServer() {
    for (std::vector<ServerConfig>::iterator i = _servers.begin(); i != _servers.end(); ++i) {
        if (listen(i->getListenFd(), 200) == -1) {
            // Logger::logMsg(RED, CONSOLE_OUTPUT, "listen error: %s", strerror(errno));
            exit(1);
        }
        if (setNonBlocking(i->getListenFd()) == -1) {
            exit(1);
        }
        addToEpoll(i->getListenFd(), EPOLLIN);
        _serversMap.insert(std::make_pair(i->getListenFd(), *i));
    }
}


// void ServerSocket::parseRequest(Client* client, const std::string& data) {
//     size_t pos = data.find("\r\n\r\n");
//     if (pos != std::string::npos) {
//         std::string request = data.substr(0, pos + 4);
//         std::string temp = data.substr(pos + 4);
//         client->setTime();
//         HTTP::Request httpRequest = HTTP::Request::deserialize(request);
//         if (httpRequest.getResource().find("/cgi-bin/") != std::string::npos) {
//             client->setCgiRequest(true);
//             ServerConfig serv = client->getServer();
//             std::vector<Location> locations = serv.getLocations();
//             std::vector<Location>::const_iterator i = std::find_if(locations.begin(), locations.end(), ServerConfig::MatchLocation("/cgi-bin/"));
//             CgiHandler* cgiHandler = new CgiHandler(client, httpRequest, httpRequest.getResource(), client->getServer(), *i, this);
//             client->setCgiHandler(cgiHandler);
//             handleCgiRequest(client);
//         } else {
//             RequestHandler handler;
//             handler.setServer(client->getServer());
//             handler.setRequest(httpRequest);
//             handler.setErrorPages(client->getServer().getErrorPages());
//             handler.setClient(client);
//             handler.handleRequest();
//             client->addResponse(handler.getResponse().serialize());
//         }
//         // client->setIncompleteRequest(temp);
//         modifyEpoll(client->getSocket(), EPOLLOUT);
//     }
// }

// void ServerSocket::handleCgiEvent(int fd, uint32_t events) {
//     CgiHandler* cgiHandler = _cgiPipeMap[fd];
//     if (events & EPOLLIN) {
//         cgiHandler->readCgiResponse();
//         // After reading the CGI response, update the epoll instance for the client socket
//         modifyEpoll(cgiHandler->getClient()->getSocket(), EPOLLOUT);
//     } else if (events & EPOLLOUT)
//         cgiHandler->sendCgiBody();
//     else
//         closeConnection(fd);
// }


// void ServerSocket::handleCgiRequest(Client* client) {
//     CgiHandler* cgiHandler = client->getCgiHandler();
//     if (!cgiHandler) {
//         Logger::logMsg(RED, CONSOLE_OUTPUT, "CgiHandler is NULL for client fd: %d", client->getSocket());
//         return;
//     }
//     cgiHandler->initCgi();
//     int pipeOutFd = cgiHandler->getPipeOut();
//     if (!_cgiPipeMap.count(pipeOutFd)) {
//         client->addFdToMonitor(pipeOutFd, EPOLLIN | EPOLLET);
//         getCgiPipeMap()[pipeOutFd] = cgiHandler;
//     }
// }


/* Send request body to CGI script */
void    ServerSocket::sendCgiBody(Client &client, CgiHandler &cgi)
{
    int bytesSent;
    std::string &reqBody = client.request.getBody();

    if (reqBody.length() == 0)
        bytesSent = 0;
    else if (reqBody.length() >= BUFF_SIZE)
        bytesSent = write(cgi.pipe_in[1], reqBody.c_str(), BUFF_SIZE);
    else
        bytesSent = write(cgi.pipe_in[1], reqBody.c_str(), reqBody.length());

    if (bytesSent < 0)
    {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "sendCgiBody() Error Sending: %s", strerror(errno));
        removeFromEpoll(cgi.pipe_in[1]);
        close(cgi.pipe_in[1]);
        close(cgi.pipe_out[1]);
        client.response.setErrorResponse(500);
    }
    else if (bytesSent == 0 || (size_t) bytesSent == reqBody.length())
    {
        removeFromEpoll(cgi.pipe_in[1]);
        close(cgi.pipe_in[1]);
        close(cgi.pipe_out[1]);
    }
    else
    {
        client.updateTime();
        reqBody = reqBody.substr(bytesSent);
    }
}



void ServerSocket::runServer() {
    listenServer();
    struct epoll_event events[MAX_EVENTS];

    while (true) {
        int numEvents = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT_PERIOD);
        if (numEvents == -1) {
            if (errno == EINTR) {
                continue;
            }
            Logger::logMsg(RED, CONSOLE_OUTPUT, "epoll_wait error: %s", strerror(errno));
            exit(1);
        }
        for (int i = 0; i < numEvents; ++i) {
            int fd = events[i].data.fd;

            if (_serversMap.find(fd) != _serversMap.end()) {
                acceptNewConnection(_serversMap[fd]);
            } else if (events[i].events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
                closeConnection(fd);
            } else {
                int cgiState = _clientsMap[fd].response.getCgiState();
                if (events[i].events & EPOLLIN) {
                    if (cgiState == 1 && events[i].data.fd == _clientsMap[fd].response._cgiObj.pipe_in[1]) {
                        sendCgiBody(_clientsMap[fd], _clientsMap[fd].response._cgiObj);
                    } else if (cgiState == 1 && events[i].data.fd == _clientsMap[fd].response._cgiObj.pipe_out[0]) {
                        readCgiResponse(_clientsMap[fd], _clientsMap[fd].response._cgiObj);
                    } else {
                        readRequest(fd, _clientsMap[fd]);
                    }
                } else if (events[i].events & EPOLLOUT) {
                    if (cgiState == 1) {
                        sendCgiBody(_clientsMap[fd], _clientsMap[fd].response._cgiObj);
                    } else if (cgiState == 2 || cgiState == 0) {
                        sendResponse(fd, _clientsMap[fd]);
                    }
                }
            }
        }

        checkTimeout();
    }
}

void    ServerSocket::sendResponse(const int &fd, Client &client)
{
    int bytesSent;
    std::string response = client.response.getRes();
    if (response.length() >= BUFF_SIZE)
        bytesSent = write(fd, response.c_str(), BUFF_SIZE);
    else
        bytesSent = write(fd, response.c_str(), response.length());

    if (bytesSent < 0)
    {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "sendResponse(): error sending : %s", strerror(errno));
        closeConnection(fd);
    }
    else if (bytesSent == 0 || (size_t) bytesSent == response.length())
    {
        // Logger::logMsg(LIGHTMAGENTA, CONSOLE_OUTPUT, "sendResponse() Done sending ");
        Logger::logMsg(CYAN, CONSOLE_OUTPUT, "Response Sent To Socket %d, Stats=<%d>"
        , fd, client.response.getCode());
        if (client.request.keepAlive() == false || client.request.errorCode() || client.response.getCgiState())
        {
            Logger::logMsg(YELLOW, CONSOLE_OUTPUT, "Client %d: Connection Closed.", fd);
            closeConnection(fd);
        }
        else
        {
            removeFromEpoll(fd);
            addToEpoll(fd, EPOLLIN);
            client.clearClient();
        }
    }
    else
    {
        client.updateTime();
        client.response.cutRes(bytesSent);
    }
}

/* Reads outpud produced by the CGI script */
void    ServerSocket::readCgiResponse(Client &client, CgiHandler &cgi)
{
    char    buffer[BUFF_SIZE * 2];
    int     bytesRead = 0;
    bytesRead = read(cgi.pipe_out[0], buffer, BUFF_SIZE* 2);

    if (bytesRead == 0)
    {
        removeFromEpoll(cgi.pipe_out[0]);
        close(cgi.pipe_in[0]);
        close(cgi.pipe_out[0]);
		int status;
		waitpid(cgi.getCgiPid(), &status, 0);
		if(WEXITSTATUS(status) != 0)
		{
			client.response.setErrorResponse(502);
		}
        client.response.setCgiState(2);
        if (client.response._responseContent.find("HTTP/1.1") == std::string::npos)
		    client.response._responseContent.insert(0, "HTTP/1.1 200 OK\r\n");
        return ;
    }
    else if (bytesRead < 0)
    {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "readCgiResponse() Error Reading From CGI Script: ", strerror(errno));
        removeFromEpoll(cgi.pipe_out[0]);
        close(cgi.pipe_in[0]);
        close(cgi.pipe_out[0]);
        client.response.setCgiState(2);
        client.response.setErrorResponse(500);
        return ;
    }
    else
    {
        client.updateTime();
		client.response._responseContent.append(buffer, bytesRead);
		memset(buffer, 0, sizeof(buffer));
    }
}

void ServerSocket::readRequest(const int &fd, Client& client) {
    char buf[BUFF_SIZE];
    ssize_t count = recv(fd, buf, BUFF_SIZE, 0);

    if (count == 0) {
        Logger::logMsg(YELLOW, CONSOLE_OUTPUT, "webserv: Client %d Closed Connection", fd);
        closeConnection(fd);
        return;
    } else if (count < 0) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "webserv: fd %d read error %s", fd, strerror(errno));
        closeConnection(fd);
    } else if (count != 0) {
        client.updateTime();
        client.request.feed(buf, count);
        memset(buf, 0, BUFF_SIZE);
    }
    if (client.request.isComplete() || client.request.errorCode()) { // 1 = parsing completed and we can work on the response.
        assignServer(client);
        Logger::logMsg(CYAN, CONSOLE_OUTPUT, "Request Recived From Socket %d, Method=<%s>  URI=<%s>"
        , fd, client.request.getMethodStr().c_str(), client.request.getPath().c_str());
        client.buildResponse();
        if (client.response.getCgiState()) {
            handleReqBody(client);
            modifyEpoll(client.response._cgiObj.pipe_in[1], EPOLLOUT);
            modifyEpoll(client.response._cgiObj.pipe_out[0], EPOLLIN);
        }
        removeFromEpoll(fd);
        addToEpoll(fd, EPOLLOUT);
    }
}

void    ServerSocket::handleReqBody(Client &client)
{
    	if (client.request.getBody().length() == 0)
		{
			std::string tmp;
			std::fstream file;(client.response._cgiObj.getCgiPath().c_str());
			std::stringstream ss;
			ss << file.rdbuf();
			tmp = ss.str();
			client.request.setBody(tmp);
		}
}

/* Assigen server_block configuration to a client based on Host Header in request and server_name*/
void    ServerSocket::assignServer(Client &c)
{
    for (std::vector<ServerConfig>::iterator it = _servers.begin();
        it != _servers.end(); ++it)
    {
        if (c.server.getHost() == it->getHost() &&
            c.server.getPort() == it->getPort() &&
            c.request.getServerName() == it->getServerName())
        {
            c.setServer(*it);
            return ;
        }
    }
}

void ServerSocket::closeConnection(int clientFd) {
    removeFromEpoll(clientFd);
    close(clientFd);
    _clientsMap.erase(clientFd);
}

void ServerSocket::setupServer(std::vector<ServerConfig> serv) {
    _servers = serv;
    for (std::vector<ServerConfig>::iterator server = _servers.begin(); server != _servers.end(); ++server) {
        server->bindServer();
        // Logger::logMsg(BLUE, CONSOLE_OUTPUT, "Server created on port: %d", server->getPort());
    }
}

/* Checks time passed for clients since last message, If more than CONNECTION_TIMEOUT, close connection */
void    ServerSocket::checkTimeout()
{
    for(std::map<int, Client>::iterator it = _clientsMap.begin() ; it != _clientsMap.end(); ++it)
    {
        if (time(NULL) - it->second.getLastTime() > CONNECTION_TIMEOUT)
        {
            Logger::logMsg(YELLOW, CONSOLE_OUTPUT, "Client %d Timeout, Closing Connection..", it->first);
            closeConnection(it->first);
            return ;
        }
    }
}

void ServerSocket::removeFdFromMonitor(int fd) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}
int ServerSocket::getEpollFd() const {
    return epoll_fd;
}
