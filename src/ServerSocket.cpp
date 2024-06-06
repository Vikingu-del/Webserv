/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:02:11 by kilchenk          #+#    #+#             */
/*   Updated: 2024/06/06 14:22:58 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include "RequestHandler.hpp"
#include <sys/epoll.h>
#include <signal.h>

// try to execute while server is open with this command in terminal siege -c50 -t30S http://localhost:8002

// Helper function to set a file descripto to non-blocking mode
int setNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl F_GETFL error");
		return -1;
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("fcntl F_SETFL error");
		return -1;
	}
	return 0;
}

ServerSocket::ServerSocket() {
    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd == -1) {
        perror("epoll_create1 error");
        exit(1);
    }
}

ServerSocket::~ServerSocket() {
	close(epoll_fd);
}

void	ServerSocket::addToEpoll(const int fd, uint32_t events)
{
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		perror("Failed to add server socket to epoll instance.");
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
		std::cerr << "epoll_ctl EPOLL_CTL_MOD error" << std::endl;
		exit(1);
	}
}
void	ServerSocket::removeFromEpoll(int fd) {
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, 0) == -1) {
		std::cerr << "epoll_ctl EPOLL_CTL_DEL error" << std::endl;
	}
	close(fd); // Close the file descriptor after removing it from epoll
	std::cout << "Removed from epoll " << fd << std::endl;
}
void	ServerSocket::acceptNewConnection(ServerConfig &serv)// we need it to  allows the server to maintain an up-to-date record of connected clients and their associated sockets for further communication and management.
{
	struct		sockaddr_in client_addr;
	socklen_t	client_addr_len = sizeof(client_addr); 
	int			client_socket = accept(serv.getListenFd(), (struct sockaddr *)&client_addr, &client_addr_len);
	if (client_socket == -1) {
		perror("accept error");
		return ;
	}
	if (setNonBlocking(client_socket) == -1) {
		close(client_socket);
		return;
	}
	// Instantiate Client with ServerConfig and set the socket
	Client	new_client(serv);
	new_client.setSocket(client_socket);
	new_client.setTime();
	// Ensure the client socket is unique in _clients_map
	if (_clientsMap.count(client_socket) != 0)
		_clientsMap.erase(client_socket);
	_clientsMap.insert(std::make_pair(client_socket, new_client));
    std::cout << "Client connected with the socket " << client_socket << std::endl;
	// Add the client socket to the epoll instance
	addToEpoll(client_socket, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
}
void	ServerSocket::listenServer()
{
	std::cout << YELLOW << "Listening to server" << RESET << std::endl;
	for (std::vector<ServerConfig>::iterator i = _servers.begin(); i != _servers.end(); ++i)
	{
		if ( listen( i->getListenFd(), 200) == -1) {
			perror("listen error");
			exit(1);
		}
		if (setNonBlocking(i->getListenFd()) == -1) {
			exit(1);
		}
		addToEpoll(i->getListenFd(), EPOLLIN);
		_serversMap.insert(std::make_pair(i->getListenFd(), *i));
	}
}
void ServerSocket::readRequest(const int &fd, Client &client, RequestHandler &handler) {
    // Check if the client socket is properly initialized and connected
	char buf[1024];
	ssize_t count = recv(fd, buf, sizeof(buf), 0);
	if (count == -1) {
		if (errno == EAGAIN) {
			perror("recv error");
			removeFromEpoll(fd);
			_clientsMap.erase(fd);
		}
		return ;
	} else if (count == 0) {
        removeFromEpoll(fd);
		_clientsMap.erase(fd);
		return ;
    } else {
		std::string temp = client.getIncompleteRequest() + std::string(buf, count);
		// std::cout << YELLOW << "Received request: " << temp << RESET << std::endl;
		size_t pos;
		while ((pos = temp.find("\r\n\r\n")) != std::string::npos) {
			std::string request = temp.substr(0, pos + 4);
			temp = temp.substr(pos + 4);
			client.setTime();
			handler.setServer(client.getServer());
			handler.setRequest(HTTP::Request::deserialize(request));
			handler.setErrorPages(client.getServer().getErrorPages());
			handler.handleRequest();
			std::cout << YELLOW << "Request handled" << RESET << std::endl;
			client.addResponse(handler.getResponse().serialize());
		}
		client.setIncompleteRequest(temp);
		modifyEpoll(fd, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
	}
}
void ServerSocket::sendResponse(const int &fd, Client &client) {
    while (client.hasResponses()) {
		std::string &buffer = client.getCurrentResponse();
		ssize_t count = send(fd, buffer.c_str(), buffer.size(), 0);
		if (count == -1) {
			if (errno != EAGAIN) {
				perror("write error");
				removeFromEpoll(fd);
				_clientsMap.erase(fd);
			}
			return;
		} else {
			buffer.erase(0, count);
			if (buffer.empty())
				client.removeCurrentResponse();
		}
	}
	if (!client.hasResponses()) {
		modifyEpoll(fd, EPOLLIN | EPOLLRDHUP);
	}
	std::cout << YELLOW << "Response sent" << RESET << std::endl;
}
void ServerSocket::closeConnection(int fd)
{
	removeFromEpoll(fd);
	_clientsMap.erase(fd);
	std::cout << YELLOW << "Connection with fd = " << fd <<  " closed" << RESET << std::endl;
}
void ServerSocket::runServer()
{
    listenServer();
	const int MAX_EVENTS = 1000;
	struct epoll_event events[MAX_EVENTS];
	time_t last_check_time = time(NULL);
	RequestHandler handler;
    while (42) {
        int numEvents = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000);
		if (numEvents == -1) {
			if (errno == EINTR) {
				continue;
			}
			perror("epoll_wait error");
			exit(1);
		}
		for (int i = 0; i < numEvents; ++i) {
			int fd = events[i].data.fd;
			if (_serversMap.find(fd) != _serversMap.end()) {
        		acceptNewConnection(_serversMap[fd]);
			} else if (events[i].events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
				closeConnection(fd);
			} else if (events[i].events & EPOLLIN) {
				readRequest(fd, _clientsMap[fd], handler);
			} else if (events[i].events & EPOLLOUT) {
				sendResponse(fd, _clientsMap[fd]);;
			} else {
				std::cerr << "Unknown event" << std::endl;
			}
		}
    }
	// Check for timeouts every second
	time_t currentTime = time(NULL);
	if (difftime(currentTime, last_check_time) > 1.0) {
		checkTimeout();
		last_check_time = currentTime;
	}
}
void ServerSocket::setupServer(std::vector<ServerConfig> serv) {
	_servers = serv;
	for (std::vector<ServerConfig>::iterator i = _servers.begin(); i != _servers.end(); ++i)
    {
        i->bindServer();
        std::cout << "Server created on port: " << i->getPort() << std::endl;
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
			std::map<int, Client>::iterator temp = it;
            ++it;
			_clientsMap.erase(temp);
        } else
            ++it;
    }
}
