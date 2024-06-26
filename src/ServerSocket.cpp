#include "Webserv.hpp"

int setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "fcntl F_GETFL error: %s", strerror(errno));
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "fcntl F_SETFL error: %s", strerror(errno));
        return -1;
    }
    return 0;
}

void ServerSocket::addToEpoll(const int fd, uint32_t events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "Failed to add fd %d to epoll instance with events %u: %s", fd, events, strerror(errno));
        close(fd);
    } else {
        Logger::logMsg(GREEN, CONSOLE_OUTPUT, "Added fd %d to epoll with events %u", fd, events);
    }
}

void ServerSocket::modifyEpoll(int fd, uint32_t events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "epoll_ctl EPOLL_CTL_MOD error for fd %d: %s", fd, strerror(errno));
        close(fd);
    } else {
        Logger::logMsg(GREEN, CONSOLE_OUTPUT, "Modified fd %d in epoll with events %u", fd, events);
    }
}

void ServerSocket::removeFromEpoll(int fd) {
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, 0) == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "epoll_ctl EPOLL_CTL_DEL error for fd %d: %s", fd, strerror(errno));
    } else {
        Logger::logMsg(GREEN, CONSOLE_OUTPUT, "Removed fd %d from epoll", fd);
    }
    close(fd);
}

// Adapted acceptNewConnection function for epoll
void ServerSocket::acceptNewConnection(ServerConfig& serv) {
    struct sockaddr_in clientAddress;
    long clientAddressSize = sizeof(clientAddress);
    int clientSocket = accept4(serv.getListenFd(), (struct sockaddr *)&clientAddress, (socklen_t*)&clientAddressSize, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (clientSocket == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "Failed to accept new connection: %s", strerror(errno));
        return;
    }
    char buffer[INET_ADDRSTRLEN]; // IPv4 address string size is 16
    Logger::logMsg(YELLOW, CONSOLE_OUTPUT, "New Connection From %s, Assigned Socket %d",inet_ntop(AF_INET, &clientAddress, buffer, INET_ADDRSTRLEN), clientSocket);
    Client newClient(serv);
    addToEpoll(clientSocket, EPOLLIN);
    newClient.setSocket(clientSocket);
    _clientsMap[clientSocket] = newClient;
}

// Why epoll_create1 and not epoll_create? epoll_create1 can take an extra flag as a parameter to set the close-on-exec flag. This flag is useful when the file descriptor is passed to a child process. The close-on-exec flag ensures that the file descriptor is closed when the child process is executed. This is useful to prevent file descriptor leaks in the child process. Also Security, prevents CGI scripts from unintentionally interacting with the epoll file descriptor, which, could lead to security vulnerabilities.
ServerSocket::ServerSocket() {
    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "epoll_create1 error: %s", strerror(errno));
        exit(1);
    }
    Logger::logMsg(GREEN, CONSOLE_OUTPUT, "Epoll instance created with fd %d", epoll_fd);
}

ServerSocket::~ServerSocket() {
    close(epoll_fd);
    Logger::logMsg(GREEN, CONSOLE_OUTPUT, "Epoll instance closed with fd %d", epoll_fd);
}

// when we are calling the listen system call based on the trafic you plan to have on your server, you can set the backlog parameter to a value that makes sense. The backlog parameter is the maximum length to which the queue of pending connections for sockfd may grow. You can check the limits your system has by cating cat /proc/sys/net/core/somaxconn and adjusting the backlog parameter accordingly.
void ServerSocket::listenServers() {
    int backlog = 1024;
    for (std::vector<ServerConfig>::iterator server = _servers.begin(); server != _servers.end(); ++server) {
        if (listen(server->getListenFd(), backlog) == -1) {
            Logger::logMsg(RED, CONSOLE_OUTPUT, "listen error: %s", strerror(errno));
            exit(1);
        }
        if (setNonBlocking(server->getListenFd()) == -1) {
            exit(1);
        }
        addToEpoll(server->getListenFd(), EPOLLIN);
        _serversMap.insert(std::make_pair(server->getListenFd(), *server));
    }
}

/* Send request body to CGI script */
void ServerSocket::sendCgiBody(Client &client, CgiHandler &cgi) {
    int bytesSent;
    std::string &reqBody = client.request.getBody();

    if (reqBody.length() == 0)
        bytesSent = 0;
    else if (reqBody.length() >= BUFF_SIZE)
        bytesSent = write(cgi.pipe_in[1], reqBody.c_str(), BUFF_SIZE);
    else
        bytesSent = write(cgi.pipe_in[1], reqBody.c_str(), reqBody.length());

    if (bytesSent < 0) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "sendCgiBody() Error Sending: %s", strerror(errno));
        removeFromEpoll(cgi.pipe_in[1]);
        close(cgi.pipe_in[1]);
        close(cgi.pipe_out[1]);
        client.response.setErrorResponse(500);
    } else if (bytesSent == 0 || (size_t) bytesSent == reqBody.length()) {
        removeFromEpoll(cgi.pipe_in[1]);
        close(cgi.pipe_in[1]);
        close(cgi.pipe_out[1]);
    } else {
        client.updateTime();
        reqBody = reqBody.substr(bytesSent);
    }
}

void ServerSocket::handleEpollIn(int fd) {
    if (_serversMap.count(fd))
        acceptNewConnection(_serversMap[fd]);
    else if (_clientsMap.count(fd)) {
        int cgiState = _clientsMap[fd].response.getCgiState();
        if (cgiState == 1 && fd == _clientsMap[fd].response._cgiObj.pipe_out[0])
            readCgiResponse(_clientsMap[fd], _clientsMap[fd].response._cgiObj);
        else
            readRequest(fd, _clientsMap[fd]);
    }
}

void ServerSocket::handleEpollOut(int fd) {
    if (_clientsMap.count(fd)) {
        int cgiState = _clientsMap[fd].response.getCgiState();
        if (cgiState == 1 && fd == _clientsMap[fd].response._cgiObj.pipe_in[1]) {
            sendCgiBody(_clientsMap[fd], _clientsMap[fd].response._cgiObj);
        } else {
            sendResponse(fd, _clientsMap[fd]);
        }
    }
}

void ServerSocket::runServers() {
    listenServers();
    struct epoll_event events[MAX_EVENTS];

    while (true) {
        int numEvents = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT_PERIOD);
        if (numEvents == -1) {
            if (errno == EINTR) {
                Logger::logMsg(YELLOW, CONSOLE_OUTPUT, "epoll_wait interrupted");
                continue;
            }
            Logger::logMsg(RED, CONSOLE_OUTPUT, "epoll_wait error: %s", strerror(errno));
            exit(1);
        }
        for (int i = 0; i < numEvents; ++i) {
            int fd = events[i].data.fd;
            if (events[i].events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR))
                closeConnection(fd);
            else if (events[i].events & EPOLLIN)
                handleEpollIn(fd);
            else if (events[i].events & EPOLLOUT)
                handleEpollOut(fd);
            else
                Logger::logMsg(RED, CONSOLE_OUTPUT, "Unknown event for fd %d", fd);
        }
        checkTimeout();
    }
}


void ServerSocket::sendResponse(const int &fd, Client &client) {
    int bytesSent;
    std::string response = client.response.getRes();
    if (response.length() >= BUFF_SIZE)
        bytesSent = write(fd, response.c_str(), BUFF_SIZE);
    else
        bytesSent = write(fd, response.c_str(), response.length());

    if (bytesSent < 0) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "sendResponse(): error sending : %s", strerror(errno));
        closeConnection(fd);
    } else if (bytesSent == 0 || (size_t) bytesSent == response.length()) {
        Logger::logMsg(CYAN, CONSOLE_OUTPUT, "Response Sent To Socket %d, Stats=<%d>", fd, client.response.getCode());
        if (client.request.keepAlive() == false || client.request.errorCode() || client.response.getCgiState()) {
            Logger::logMsg(YELLOW, CONSOLE_OUTPUT, "Client %d: Connection Closed.", fd);
            closeConnection(fd);
        } else {
            removeFromEpoll(fd);
            addToEpoll(fd, EPOLLIN);
            client.clearClient();
        }
    } else {
        client.updateTime();
        client.response.cutRes(bytesSent);
    }
}

/* Reads outpud produced by the CGI script */
void ServerSocket::readCgiResponse(Client &client, CgiHandler &cgi) {
    char buffer[BUFF_SIZE * 2];
    int bytesRead = read(cgi.pipe_out[0], buffer, BUFF_SIZE * 2);

    if (bytesRead == 0) {
        removeFromEpoll(cgi.pipe_out[0]);
        close(cgi.pipe_in[0]);
        close(cgi.pipe_out[0]);
        int status;
        waitpid(cgi.getCgiPid(), &status, 0);
        if (WEXITSTATUS(status) != 0) {
            client.response.setErrorResponse(502);
        }
        client.response.setCgiState(2);
        if (client.response._responseContent.find("HTTP/1.1") == std::string::npos)
            client.response._responseContent.insert(0, "HTTP/1.1 200 OK\r\n");
        return;
    } else if (bytesRead < 0) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "readCgiResponse() Error Reading From CGI Script: %s", strerror(errno));
        removeFromEpoll(cgi.pipe_out[0]);
        close(cgi.pipe_in[0]);
        close(cgi.pipe_out[0]);
        client.response.setCgiState(2);
        client.response.setErrorResponse(500);
        return;
    } else {
        client.updateTime();
        client.response._responseContent.append(buffer, bytesRead);
        memset(buffer, 0, sizeof(buffer));
    }
}

void ServerSocket::readRequest(const int &fd, Client& client) {
    char buf[BUFF_SIZE];
    int count = read(fd, buf, BUFF_SIZE);
    if (count == 0) {
        Logger::logMsg(YELLOW, CONSOLE_OUTPUT, "webserv: Client %d Closed Connection", fd);
        closeConnection(fd);
        return;
    } else if (count < 0) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "webserv: fd %d read error %s", fd, strerror(errno));
        closeConnection(fd);
        return;
    } else {
        client.updateTime();
        client.request.feed(buf, count);
        memset(buf, 0, BUFF_SIZE);
    }
    if (client.request.isComplete() || client.request.errorCode()) {
        assignServer(client);
        Logger::logMsg(CYAN, CONSOLE_OUTPUT, "Request Recived From Socket %d, Method=<%s>  URI=<%s>", fd, client.request.getMethodStr().c_str(), client.request.getPath().c_str());
        client.buildResponse();
        if (client.response.getCgiState()) {
            handleReqBody(client);
            addToEpoll(client.response._cgiObj.pipe_in[1], EPOLLOUT);
            addToEpoll(client.response._cgiObj.pipe_out[0], EPOLLIN);
        }
        modifyEpoll(fd, EPOLLOUT);
    }
}

void ServerSocket::handleReqBody(Client &client) {
    if (client.request.getBody().length() == 0) {
        std::string tmp;
        std::fstream file(client.response._cgiObj.getCgiPath().c_str());
        std::stringstream ss;
        ss << file.rdbuf();
        tmp = ss.str();
        client.request.setBody(tmp);
    }
}

/* Assigen server_block configuration to a client based on Host Header in request and server_name*/
void ServerSocket::assignServer(Client &c) {
    for (std::vector<ServerConfig>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
        if (c.server.getHost() == it->getHost() && c.server.getPort() == it->getPort() && c.request.getServerName() == it->getServerName()) {
            c.setServer(*it);
            return;
        }
    }
}

void ServerSocket::closeConnection(int clientFd) {
    if (_clientsMap.find(clientFd) != _clientsMap.end()) {
        removeFromEpoll(clientFd);
        _clientsMap.erase(clientFd);
    } else
        Logger::logMsg(YELLOW, CONSOLE_OUTPUT, "Attempted to close unknown fd %d", clientFd);
}

void ServerSocket::setupServers(std::vector<ServerConfig> serv) {
    _servers = serv;
    for (std::vector<ServerConfig>::iterator server = _servers.begin(); server != _servers.end(); ++server) {
        server->bindServer();
    }
}

/* Checks time passed for clients since last message, If more than CONNECTION_TIMEOUT, close connection */
void ServerSocket::checkTimeout() {
    for (std::map<int, Client>::iterator it = _clientsMap.begin(); it != _clientsMap.end(); ++it) {
        if (time(NULL) - it->second.getLastTime() > CONNECTION_TIMEOUT) {
            Logger::logMsg(YELLOW, CONSOLE_OUTPUT, "Client %d Timeout, Closing Connection..", it->first);
            closeConnection(it->first);
            return;
        }
    }
}

// void ServerSocket::removeFdFromMonitor(int fd) {
//     epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
// }

int ServerSocket::getEpollFd() const {
    return epoll_fd;
}