#include "ServerSocket.hpp"
#include "Logger.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <ctime>
#include <algorithm>
#include "Client.hpp"
#include "CgiHandler.hpp"
#include "RequestHandler.hpp"

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

ServerSocket::ServerSocket() {
    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "epoll_create1 error: %s", strerror(errno));
        exit(1);
    }
}

ServerSocket::~ServerSocket() {
    for (std::map<int, Client*>::iterator it = _clientsMap.begin(); it != _clientsMap.end(); ++it) {
        delete it->second;
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
    Logger::logMsg(RED, CONSOLE_OUTPUT, "Added fd %d to epoll with events %u", fd, events);
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

void ServerSocket::acceptNewConnection(ServerConfig& serverConfig) {
    int clientFd = accept(serverConfig.getListenFd(), NULL, NULL);
    if (clientFd == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "Failed to accept new connection: %s", strerror(errno));
        return;
    }

    setNonBlocking(clientFd);

    // Use addToEpoll to add the client file descriptor to the epoll instance
    addToEpoll(clientFd, EPOLLIN);

    _clientsMap[clientFd] = new Client(serverConfig, *this, clientFd);
    Logger::logMsg(RED, CONSOLE_OUTPUT, "Accepted new connection, fd: %d", clientFd);
}

void ServerSocket::listenServer() {
    for (std::vector<ServerConfig>::iterator i = _servers.begin(); i != _servers.end(); ++i) {
        if (listen(i->getListenFd(), 200) == -1) {
            Logger::logMsg(RED, CONSOLE_OUTPUT, "listen error: %s", strerror(errno));
            exit(1);
        }
        if (setNonBlocking(i->getListenFd()) == -1) {
            exit(1);
        }
        addToEpoll(i->getListenFd(), EPOLLIN);
        _serversMap.insert(std::make_pair(i->getListenFd(), *i));
    }
}

void ServerSocket::readRequest(int fd, Client* client) {
    char buf[1024];
    ssize_t count = recv(fd, buf, sizeof(buf), 0);

    if (count == -1) {
        if (errno == EAGAIN) {
            Logger::logMsg(RED, CONSOLE_OUTPUT, "recv error: %s", strerror(errno));
            closeConnection(fd);
        }
        return;
    } else if (count == 0) {
        closeConnection(fd);
        return;
    } else {
        std::string temp = client->getIncompleteRequest() + std::string(buf, count);
        parseRequest(client, temp);
    }
}

void ServerSocket::parseRequest(Client* client, const std::string& data) {
    size_t pos = data.find("\r\n\r\n");
    if (pos != std::string::npos) {
        std::string request = data.substr(0, pos + 4);
        std::string temp = data.substr(pos + 4);
        client->setTime();
        HTTP::Request httpRequest = HTTP::Request::deserialize(request);
        if (httpRequest.getResource().find("/cgi-bin/") != std::string::npos) {
            client->setCgiRequest(true);
            ServerConfig serv = client->getServer();
            std::vector<Location> locations = serv.getLocations();
            std::vector<Location>::const_iterator i = std::find_if(locations.begin(), locations.end(), ServerConfig::MatchLocation("/cgi-bin/"));
            CgiHandler* cgiHandler = new CgiHandler(client, httpRequest, httpRequest.getResource(), client->getServer(), *i, this);
            client->setCgiHandler(cgiHandler);
            handleCgiRequest(client);
        } else {
            RequestHandler handler;
            handler.setServer(client->getServer());
            handler.setRequest(httpRequest);
            handler.setErrorPages(client->getServer().getErrorPages());
            handler.setClient(client);
            handler.handleRequest();
            client->addResponse(handler.getResponse().serialize());
        }
        // client->setIncompleteRequest(temp);
        modifyEpoll(client->getSocket(), EPOLLOUT);
    }
}

void ServerSocket::handleCgiEvent(int fd, uint32_t events) {
    Logger::logMsg(RED, CONSOLE_OUTPUT, "Handling CGI event for fd: %d, events: %u", fd, events);
    CgiHandler* cgiHandler = _cgiPipeMap[fd];
    if (events & EPOLLIN) {
        std::cout << "Reading CGI response" << std::endl;
        cgiHandler->readCgiResponse();
    } else if (events & EPOLLOUT) {
        std::cout << "Sending CGI body" << std::endl;
        cgiHandler->sendCgiBody();
    } else {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "Closing CGI connection due to hang up or error for fd: %d", fd);
        closeConnection(fd);
    }
}

void ServerSocket::removeFdFromMonitor(int fd) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

void ServerSocket::handleCgiRequest(Client* client) {
    Logger::logMsg(RED, CONSOLE_OUTPUT, "handleCgiRequest for client fd: %d", client->getSocket());
    CgiHandler* cgiHandler = client->getCgiHandler();
    if (!cgiHandler) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "CgiHandler is NULL for client fd: %d", client->getSocket());
        return;
    }

    cgiHandler->initCgi();
    client->addFdToMonitor((*cgiHandler).getPipeOut(), EPOLLOUT | EPOLLET);
    getCgiPipeMap()[(*cgiHandler).getPipeOut()] = cgiHandler;
    Logger::logMsg(RED, CONSOLE_OUTPUT, "execCgi called for client fd: %d", client->getSocket());
}

void ServerSocket::sendResponse(const int fd, Client* client) {
    while (client->hasResponses()) {
        std::string& buffer = client->getCurrentResponse();
        ssize_t count = send(fd, buffer.c_str(), buffer.size(), 0);
        if (count == -1) {
            if (errno != EAGAIN) {
                Logger::logMsg(RED, CONSOLE_OUTPUT, "write error: %s", strerror(errno));
                closeConnection(fd);
            }
            return;
        } else {
            buffer.erase(0, count);
            if (buffer.empty()) {
                client->removeCurrentResponse();
            }
        }
    }
    if (!client->hasResponses()) {
        if (client->isCgiRequest() && client->getCgiHandler()->getState() != CgiHandler::DONE) {
            modifyEpoll(fd, EPOLLIN | EPOLLRDHUP);
        } else {
            modifyEpoll(fd, EPOLLIN | EPOLLRDHUP | EPOLLHUP);
        }
    }
    Logger::logMsg(YELLOW, CONSOLE_OUTPUT, "Response sent");
}

void ServerSocket::closeConnection(int clientFd) {
    removeFromEpoll(clientFd);
    close(clientFd);
    delete _clientsMap[clientFd];
    _clientsMap.erase(clientFd);
}

void ServerSocket::runServer() {
    listenServer();
    const int MAX_EVENTS = 1000;
    struct epoll_event events[MAX_EVENTS];
    time_t last_check_time = time(NULL);

    while (true) {
        int numEvents = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000);
        if (numEvents == -1) {
            if (errno == EINTR) {
                continue;
            }
            Logger::logMsg(RED, CONSOLE_OUTPUT, "epoll_wait error: %s", strerror(errno));
            exit(1);
        }
        for (int i = 0; i < numEvents; ++i) {
            int fd = events[i].data.fd;
            Logger::logMsg(RED, CONSOLE_OUTPUT, "Handling event for fd: %d, events: %u", fd, events[i].events);

            if (_serversMap.find(fd) != _serversMap.end()) {
                acceptNewConnection(_serversMap[fd]);
            } else if (events[i].events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
                Logger::logMsg(RED, CONSOLE_OUTPUT, "Closing connection due to hang up or error for fd: %d", fd);
                closeConnection(fd);
            } else {
                if (_clientsMap.find(fd) != _clientsMap.end()) {
                    _clientsMap[fd]->handleEvent(events[i].events);
                } else if (_cgiPipeMap.find(fd) != _cgiPipeMap.end()) {
                    handleCgiEvent(fd, events[i].events);
                } else {
                    Logger::logMsg(RED, CONSOLE_OUTPUT, "Unknown file descriptor: %d", fd);
                }
            }
        }

        time_t currentTime = time(NULL);
        if (difftime(currentTime, last_check_time) > 1.0) {
            checkTimeout();
            last_check_time = currentTime;
        }
    }
}

void ServerSocket::handleEpollIn(int fd) {
    if (_clientsMap.find(fd) != _clientsMap.end()) {
        _clientsMap[fd]->handleEvent(EPOLLIN);
    }
}

void ServerSocket::handleEpollOut(int fd) {
    if (_clientsMap.find(fd) != _clientsMap.end()) {
        _clientsMap[fd]->handleEvent(EPOLLOUT);
    }
}

void ServerSocket::setupServer(std::vector<ServerConfig> serv) {
    _servers = serv;
    for (std::vector<ServerConfig>::iterator server = _servers.begin(); server != _servers.end(); ++server) {
        server->bindServer();
        Logger::logMsg(BLUE, CONSOLE_OUTPUT, "Server created on port: %d", server->getPort());
    }
}

void ServerSocket::checkTimeout() {
    time_t currentTime = time(NULL);
    std::map<int, Client*>::iterator it = _clientsMap.begin();
    while (it != _clientsMap.end()) {
        if (difftime(currentTime, it->second->getLastTime()) > TIMEOUT_PERIOD) {
            int fd = it->first;
            Logger::logMsg(RED, CONSOLE_OUTPUT, "Client Timeout, Closing Connection");
            delete it->second;
            std::map<int, Client*>::iterator temp = it;
            ++it;
            _clientsMap.erase(temp);
            close(fd);
        } else {
            ++it;
        }
    }
}

int ServerSocket::getEpollFd() const {
    return epoll_fd;
}
