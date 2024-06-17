/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:03:11 by kilchenk          #+#    #+#             */
/*   Updated: 2024/06/17 12:31:20 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include <sys/socket.h>
#include <sys/epoll.h>
#include <map>
#include <vector>
#include "ServerConfig.hpp"
#include "Client.hpp"
#include "RequestHandler.hpp"
#include "CgiHandler.hpp"

class ServerSocket {
private:
    std::vector<ServerConfig> _servers;
    std::map<int, ServerConfig> _serversMap;
    std::map<int, Client*> _clientsMap;
    int epoll_fd;

    void parseRequest(Client *client, const std::string &data);
    void handleCgiRequest(Client *client);

public:
    ServerSocket();
    ~ServerSocket();
    void setupServer(std::vector<ServerConfig> serv);
    void runServer();
    void acceptNewConnection(ServerConfig &serv);
    void listenServer();
    void readRequest(const int &fd, Client *client);
    void sendResponse(const int &fd, Client *client);
    void closeConnection(const int fd);
    void addToEpoll(const int fd, uint32_t events);
    void modifyEpoll(int fd, uint32_t events);
    void removeFromEpoll(const int fd);
    void checkTimeout();
    int     getEpollFd() const;
};

#endif
