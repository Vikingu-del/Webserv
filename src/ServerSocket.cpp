/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:02:11 by kilchenk          #+#    #+#             */
/*   Updated: 2024/06/17 14:39:42 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include "RequestHandler.hpp"
#include <sys/epoll.h>
#include <signal.h>
#include "Logger.hpp"

// try to execute while server is open with this command in terminal siege -c50 -t30S http://localhost:8002

// Helper function to set a file descripto to non-blocking mode
int setNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) {
		Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "fcntl F_GETFL error");
		return -1;
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "fcntl F_SETFL error");
		return -1;
	}
	return 0;
}

ServerSocket::ServerSocket() {
    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd == -1) {
        Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "epoll_create1 error");
        exit(1);
    }
}

ServerSocket::~ServerSocket() {
    // Iterate through all client connections and clean up
    for (std::map<int, Client*>::iterator it = _clientsMap.begin(); it != _clientsMap.end(); ++it) {
        delete it->second; // Delete the dynamically allocated Client object
        close(it->first); // Close the client socket
    }
    _clientsMap.clear(); // Clear the map to ensure all entries are removed

    // Finally, close the epoll file descriptor
    close(epoll_fd);
}

void	ServerSocket::addToEpoll(const int fd, uint32_t events)
{
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "Failed to add server socket to epoll instance.");
		close(fd);
		close(epoll_fd);
		exit(1);
	}
	std::cout << "addet to epoll " << fd << std::endl;
}

void	ServerSocket::modifyEpoll(int fd, uint32_t events) {
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
		Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "epoll_ctl EPOLL_CTL_MOD error");
        exit(1);
	}
}
void	ServerSocket::removeFromEpoll(int fd) {
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, 0) == -1) {
		Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "epoll_ctl EPOLL_CTL_DEL error");
    }
	close(fd); // Close the file descriptor after removing it from epoll
	// std::cout << "Removed from epoll " << fd << std::endl;
}

void ServerSocket::acceptNewConnection(ServerConfig &serv) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_socket = accept(serv.getListenFd(), (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket == -1) {
        Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "Webserv: Accept Error");
        return;
    }
    if (setNonBlocking(client_socket) == -1) {
        close(client_socket);
        return;
    }
    // Instantiate Client with ServerConfig and set the socket
    Client* new_client = new Client(serv, epoll_fd);
    new_client->setSocket(client_socket);
    new_client->setTime();
    // Ensure the client socket is unique in _clientsMap
    if (_clientsMap.count(client_socket) != 0) {
        delete _clientsMap[client_socket]; // Delete existing client
        _clientsMap.erase(client_socket); // Erase from map
    }
    _clientsMap[client_socket] = new_client;
    // Add the client socket to the epoll instance
    addToEpoll(client_socket, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
}

void	ServerSocket::listenServer()
{
	// std::cout << YELLOW << "Listening to server" << RESET << std::endl;
	for (std::vector<ServerConfig>::iterator i = _servers.begin(); i != _servers.end(); ++i)
	{
		if ( listen( i->getListenFd(), 200) == -1) {
			Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "listen error");
			exit(1);
		}
		if (setNonBlocking(i->getListenFd()) == -1) {
			exit(1);
		}
		addToEpoll(i->getListenFd(), EPOLLIN);
		_serversMap.insert(std::make_pair(i->getListenFd(), *i));
	}
}

void ServerSocket::readRequest(const int &fd, Client *client) {
    char buf[1024];
    ssize_t count = recv(fd, buf, sizeof(buf), 0);

    if (count == -1) {
        if (errno == EAGAIN) {
            Logger::logMsg("ERROR", CONSOLE_OUTPUT ,"recv error: %s", strerror(errno));
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

void ServerSocket::parseRequest(Client *client, const std::string &data) {
    size_t pos;
    std::string temp = data;
    while ((pos = temp.find("\r\n\r\n")) != std::string::npos) {
        std::string request = temp.substr(0, pos + 4);
        temp = temp.substr(pos + 4);
        client->setTime();

        HTTP::Request httpRequest = HTTP::Request::deserialize(request);

        // Check if the request is for CGI
        if (httpRequest.getResource().find("/cgi-bin/") != std::string::npos) {
            client->setCgiRequest(true);
            CgiHandler* cgiHandler = new CgiHandler(client, httpRequest, client->getEpollFd(), httpRequest.getResource(), client->getServer());
            client->setCgiHandler(cgiHandler);
            handleCgiRequest(client);
        } else {
            // Handle normal request
            RequestHandler handler;
            handler.setServer(client->getServer());
            handler.setRequest(httpRequest);
            handler.setErrorPages(client->getServer().getErrorPages());
            handler.setClient(client);
            handler.handleRequest();
            client->addResponse(handler.getResponse().serialize());
        }
    }

    // Store any incomplete request back in the client
    client->setIncompleteRequest(temp);
    // Update epoll to continue monitoring the socket
    modifyEpoll(client->getSocket(), EPOLLIN | EPOLLOUT | EPOLLRDHUP);
}

void ServerSocket::handleCgiRequest(Client *client) {
    CgiHandler *cgiHandler = client->getCgiHandler();
    cgiHandler->execCgi();

    // Monitor the pipes for the CGI handler
    addToEpoll(cgiHandler->getPipeIn(), EPOLLIN);
    addToEpoll(cgiHandler->getPipeOut(), EPOLLOUT);
}

void ServerSocket::sendResponse(const int &fd, Client *client) {
    while (client->hasResponses()) {
		std::string &buffer = client->getCurrentResponse();
		ssize_t count = send(fd, buffer.c_str(), buffer.size(), 0);
		if (count == -1) {
			if (errno != EAGAIN) {
				Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "write error");
				closeConnection(fd);
			}
			return;
		} else {
			buffer.erase(0, count);
			if (buffer.empty())
				client->removeCurrentResponse();
		}
	}
	if (!client->hasResponses()) {
		modifyEpoll(fd, EPOLLIN | EPOLLRDHUP);
	}
	// std::cout << YELLOW << "Response sent" << RESET << std::endl;
}

void ServerSocket::closeConnection(int fd)
{
    std::map<int, Client*>::iterator it = _clientsMap.find(fd);
    if (it != _clientsMap.end()) {
        delete it->second;
        _clientsMap.erase(it);
    }
    close(fd);
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
            Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "epoll_wait error");
            exit(1);
        }
        for (int i = 0; i < numEvents; ++i) {
            int fd = events[i].data.fd;
            if (_serversMap.find(fd) != _serversMap.end()) {
                acceptNewConnection(_serversMap[fd]);
            } else if (events[i].events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
                Logger::logMsg("INFO", CONSOLE_OUTPUT, "Closing connection due to hang up or error");
                closeConnection(fd);
            } else if (events[i].events & EPOLLIN) {
                std::map<int, Client*>::iterator it = _clientsMap.find(fd);
                if (it != _clientsMap.end() && it->second) { // Check if fd is found and not null
                    if (it->second->isCgiRequest()) {
                        CgiHandler *cgiHandler = it->second->getCgiHandler();
                        if (cgiHandler && cgiHandler->getState() == CgiHandler::READING) { // Also check cgiHandler is not null
                            cgiHandler->readCgiResponse(it->second, cgiHandler);
                        } else {
                            readRequest(fd, it->second);
                        }
                    } else {
                        readRequest(fd, it->second);
                    }
                } else {
                    Logger::logMsg("ERROR", CONSOLE_OUTPUT, "Client not found for EPOLLIN event");
                }
            } else if (events[i].events & EPOLLOUT) {
                std::map<int, Client*>::iterator it = _clientsMap.find(fd);
                if (it != _clientsMap.end() && it->second) {
                    if (it->second->isCgiRequest()) {
                        CgiHandler *cgiHandler = it->second->getCgiHandler();
                        if (cgiHandler->getState() == CgiHandler::WRITING) {
                            cgiHandler->sendCgiBody(it->second, cgiHandler);
                        } else {
                            sendResponse(fd, it->second);
                        }
                    } else {
                        sendResponse(fd, it->second);
                    }
                } else {
                    Logger::logMsg("ERROR", CONSOLE_OUTPUT, "Client not found for EPOLLOUT event");
                }
            } else {
                Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "Unknown event");
            }
        }

        // Check for timeouts every second
        time_t currentTime = time(NULL);
        if (difftime(currentTime, last_check_time) > 1.0) {
            checkTimeout();
            last_check_time = currentTime;
        }
    }
}

void ServerSocket::setupServer(std::vector<ServerConfig> serv) {
	_servers = serv;
	for (std::vector<ServerConfig>::iterator server = _servers.begin(); server != _servers.end(); ++server)
    {
        server->bindServer();
        Logger::logMsg("INFO", CONSOLE_OUTPUT, "Server created on port: %d", server->getPort());
    }
}

void ServerSocket::checkTimeout() {
    time_t currentTime = time(NULL);
    std::map<int, Client*>::iterator it = _clientsMap.begin();
    while (it != _clientsMap.end()) {
        if (difftime(currentTime, it->second->getLastTime()) > TIMEOUT_PERIOD) {
            int fd = it->first;
            Logger::logMsg("INFO", CONSOLE_OUTPUT, "Client Timeout, Closing Connection");
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
