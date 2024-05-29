/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:02:11 by kilchenk          #+#    #+#             */
/*   Updated: 2024/05/29 14:29:29 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include "RequestHandler.hpp"
#include <sys/epoll.h>
#include <signal.h>

// Did you ever tested with scripts from evaluation shit? (ERIK)
// Clear inside client
// try to execute while server is open with this command in terminal siege -c50 -t30S http://localhost:8002


ServerSocket::ServerSocket() {
    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd == -1) {
        perror("epoll_create1 error");
        exit(1);
    }
    // Create a new client and add it to _clientsMap
}

ServerSocket::~ServerSocket() {
	close(epoll_fd);
}

void	ServerSocket::addToEpoll(const int fd, uint32_t events)
{
	std::cout << "addet to epoll " << fd << std::endl;
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1)
	{
		perror("Failed to add server socket to epoll instance.");
		close(fd);
		close(epoll_fd);
		exit(1);
	}
}

void	ServerSocket::modifyEpoll(int fd, uint32_t events) {
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
		std::cerr << "epoll_ctl EPOLL_CTL_MOD error" << std::endl;
		exit(1);
	}
}

void	ServerSocket::removeFromEpoll(int fd) {
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, 0) == -1) {
		std::cerr << "epoll_ctl EPOLL_CTL_DEL error" << std::endl;
	}
	close(fd); // Close the file descriptor after removing it from epoll
}

void	ServerSocket::acceptNewConnection(ServerConfig &serv)// we need it to  allows the server to maintain an up-to-date record of connected clients and their associated sockets for further communication and management.
{
	std::cout << YELLOW << "Accepting new connection" << RESET << std::endl;
	struct		sockaddr_in client_addr;
	socklen_t	client_addr_len = sizeof(client_addr); 
	int			client_socket = accept(serv.getListenFd(), (struct sockaddr *)&client_addr, &client_addr_len);
	if (client_socket == -1) {
		std::cerr << "accept error" << std::endl;
		return ;
	}
	int flags = fcntl(client_socket, F_GETFL, 0);
	if (flags == -1) {
		std::cerr << "fcntl F_GETFL error" << std::endl;
		close(client_socket);
		return;
	}
	if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "fcntl F_SETFL error" << std::endl;
		close(client_socket);
		return;
	}
	// Instantiate Client with ServerConfig and set the socket
	Client	new_client(serv);
	new_client.setSocket(client_socket);

	std::cout << "Client connected with socket " << new_client.getSocket() << std::endl;

	// Ensure the client socket is unique in _clients_map
	if (_clientsMap.count(client_socket) != 0)
		_clientsMap.erase(client_socket);
	_clientsMap.insert(std::make_pair(client_socket, new_client));
	std::cout << "inserting the client into the map with <" << client_socket << ", " << serv.getPort() << ">" << std::endl;
	// Add the client socket to the epoll instance
	addToEpoll(client_socket, EPOLLIN);
}

void	ServerSocket::listenServer()
{
	std::cout << YELLOW << "Listening to server" << RESET << std::endl;
	for (std::vector<ServerConfig>::iterator i = _servers.begin(); i != _servers.end(); ++i)
	{
		if ( listen( i->getListenFd(), 10) == -1) {
			perror("listen error");
			exit(1);
		}
		int flags = fcntl(i->getListenFd(), F_GETFL, 0);
		if (flags == -1) {
			perror("fcntl F_GETFL error");
			exit(1);
		}
		if (fcntl(i->getListenFd(), F_SETFL, flags | O_NONBLOCK) == -1) {
			perror("fcntl F_SETFL error");
			exit(1);
		}
		addToEpoll(i->getListenFd(), EPOLLIN);
		_serversMap.insert(std::make_pair(i->getListenFd(), *i));
	}
}

void ServerSocket::readRequest(const int &fd, Client &client) {
    std::cout << YELLOW << "Reading request" << RESET << std::endl;

    // Check if the client socket is properly initialized and connected
    if (_serversMap.find(fd) != _serversMap.end()) {
        acceptNewConnection(_serversMap[fd]);
    }

    // Check for socket error status
    int error = 0;
    socklen_t len = sizeof(error);
    int retval = getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len);
    if (retval != 0) {
        // Log error getting socket error code
        std::cerr << "error getting socket error code: " << strerror(retval) << std::endl;
        removeFromEpoll(fd);
        return;
    }

    // Handle socket error
    if (error != 0) {
        // Log socket error
        std::cerr << "socket error: " << strerror(error) << std::endl;
        removeFromEpoll(fd);
        return;
    }

    // Read request from the client socket
    char buf[MAX_CONTENT_LENGTH];
    ssize_t count = read(fd, buf, MAX_CONTENT_LENGTH);
    if (count == -1) {
        // Handle read error
        if (errno == ECONNRESET || errno == ENOTCONN) {
			if (errno == ECONNRESET)
				perror("Connection reset by peer");
			else
            // Connection reset by peer or socket not connected
            perror("socket not connected");
            removeFromEpoll(fd);
            close(fd);
        } else if (errno != EAGAIN) {
            // Other read error
            perror("read error");
            removeFromEpoll(fd);
        }
    } else if (count == 0) {
        // Handle socket closure
        removeFromEpoll(fd);
    } else {
        // Process received data
        std::string temp = client.getIncompleteRequest() + std::string(buf, count);
        size_t pos;
        while ((pos = temp.find("\r\n\r\n")) != std::string::npos) {
            std::string request = temp.substr(0, pos + 4);
            temp = temp.substr(pos + 4);
            client.setTime();
            client.addRequest(request);
        }
        // Store any incomplete request
        client.setIncompleteRequest(temp);
        // Modify the epoll instance to also monitor for writing
        modifyEpoll(fd, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
    }
}

void ServerSocket::sendResponse(const int &fd, Client &client) {
	std::cout << YELLOW << "Sending response" << RESET << std::endl;
    while (client.hasResponses()) {
		std::string &buffer = client.getCurrentResponse();
		ssize_t count = write(fd, buffer.c_str(), buffer.size());
		if (count == -1) {
			if (errno != EAGAIN) {
				perror("write error");
				removeFromEpoll(fd);
				_clientsMap.erase(fd);
			}
			break;
		} else {
			buffer.erase(0, count);
			if (buffer.empty()) {
				client.removeCurrentResponse();
			}
		}
	}
	if (!client.hasResponses()) {
		modifyEpoll(fd, EPOLLIN | EPOLLRDHUP);
	}
}

void ServerSocket::closeConnection(int fd)
{
	std::cout << YELLOW << "Closing connection" << RESET << std::endl;
	removeFromEpoll(fd);
	_clientsMap.erase(fd);
}

void ServerSocket::runServer()
{
	std::cout << GREEN << "Server is running" << RESET << std::endl;
    listenServer();
	const int MAX_EVENTS = 10;
	struct epoll_event events[MAX_EVENTS];
	time_t last_check_time = time(NULL);
	// sigset_t mask;
	// sigemptyset(&mask);
	// sigaddset(&mask, SIGINT); // Block SIGINT during epoll_pwait
    while (true) {
		std::cout << "Waiting for events" << std::endl;
        int numEvents = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (numEvents == -1) {
			std::cerr << "Failed to wait for events." << std::endl;
            break;
		}
		for (int i = 0; i < numEvents; ++i) {
			int fd = events[i].data.fd;
			if (fd == _servers[i].getListenFd()) {
				acceptNewConnection(_serversMap[fd]);
				continue;
			}
			if (events[i].events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
				std::cerr << "Client disconnected or error occurred" << std::endl;
				closeConnection(fd);
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL); // Remove fd from epoll instance
				_clientsMap.erase(fd); // Remove fd from _clientsMap
			}
			else if (events[i].events & EPOLLIN) {
				readRequest(fd, _clientsMap[fd]);
				std::cout << "Request read" << std::endl;
			}
			else if (events[i].events & EPOLLOUT) {
				sendResponse(fd, _clientsMap[fd]);
				std::cout << "Response sent" << std::endl;
			}
			else {
				std::cerr << "Unknown event" << std::endl;
			}
		}
    }

	// Check for timeouts every second
	time_t currentTime = time(NULL);
	if (difftime(currentTime, last_check_time) > 1) {
		checkTimeout();
		last_check_time = currentTime;
	}
}


void ServerSocket::setupServer(std::vector<ServerConfig> serv) {
	_servers = serv;
	std::cout << PURPLE << "Setting up server" << std::endl;
	for (std::vector<ServerConfig>::iterator i = _servers.begin(); i != _servers.end(); ++i)
    {
        std::cout << "Binding server with port " << i->getPort() << std::endl;
        i->bindServer();
        std::cout << "Server created" << std::endl;
    }
}

void ServerSocket::checkTimeout() {
	std::cout << YELLOW << "Checking for timeouts" << RESET << std::endl;
	time_t currentTime = time(NULL);
	std::map<int, Client>::iterator it = _clientsMap.begin();
    while (it != _clientsMap.end()) {
        if (difftime(currentTime, it->second.getLastTime()) > TIMEOUT_PERIOD) {
            int fd = it->first;
            std::cerr << "Client Timeout, Closing Connection" << std::endl;
            removeFromEpoll(fd);
            _clientsMap.erase(it++);
        } else {
            ++it;
        }
    }
}
