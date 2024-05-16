/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:02:11 by kilchenk          #+#    #+#             */
/*   Updated: 2024/05/16 18:27:38 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"

ServerSocket::ServerSocket()
{

}

ServerSocket::~ServerSocket()
{
    
}

void ServerSocket::setupServer(std::vector<ServerConfig> serv)
{
    _servers = serv;
    // char buf[INET_ADDRSTRLEN];
    bool serverDup;

    for (std::vector<ServerConfig>::iterator i = _servers.begin(); i != _servers.end(); ++i)
    {
        serverDup = false;
        for (std::vector<ServerConfig>::iterator i2 = _servers.begin(); i2 != i; ++i2)
        {
            if(i2->getHost() == i->getHost() && i2->getPort() == i->getPort())
            {
                i->setFd(i2->getListenFd());
                serverDup = true;
            }
        }
        if (!serverDup)
            i->bindServer();
        //log msg wich server we are created
    }
}

void ServerSocket::runServer()
{
    //need send to cgi response
}

void ServerSocket::listenServer()
{
    FD_ZERO(&_read_fd); //+- need check
    FD_ZERO(&_write_fd);
    for (std::vector<ServerConfig>::iterator i = _servers.begin(); i != _servers.end(); ++i)
    {
        if (listen(i->getListenFd(), 420) == -1)
        {
            //listen error
            exit(1);
        }
        addToSet(i->getListenFd(), _read_fd);
        _servers_map.insert(std::make_pair(i->getListenFd(), *i)); //need to allows us to quickly look up the server associated with a file descriptor later on. 
    }
    _biggest_fd = _servers.back().getListenFd(); //need to select func
}

//next accept

void ServerSocket::addToSet(const int fd, fd_set &set)
{
    FD_SET(fd, &set);
    if (fd > _biggest_fd)
        _biggest_fd = fd;
}

void ServerSocket::removeFromSet(const int fd, fd_set &set)
{
    FD_CLR(fd, &set);
    if (fd == _biggest_fd)
        _biggest_fd--;
}
