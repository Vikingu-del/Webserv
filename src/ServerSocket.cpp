/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:02:11 by kilchenk          #+#    #+#             */
/*   Updated: 2024/05/22 15:35:28 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"
#include <cstdlib>  // note by erik: added this include to use exit(1)
#include <unistd.h>
#include <fcntl.h>

ServerSocket::ServerSocket()
{

}

ServerSocket::~ServerSocket()
{

}

// need to check
void ServerSocket::setupServer(std::vector<ServerConfig> serv)
{
    _servers = serv;
    // char buf[INET_ADDRSTRLEN];
    // bool serverDup;

    std::cout << "Setting up server" << std::endl;
    std::cout << "Server port: " << _servers[0].getPort() << std::endl;
    for (std::vector<ServerConfig>::iterator i = _servers.begin(); i != _servers.end(); ++i)
    {
        // serverDup = false;
        // for (std::vector<ServerConfig>::iterator i2 = _servers.begin(); i2 != i; ++i2)
        // {
        //     std::cout << "i: " << i->getPort() << " i2: " << i2->getPort() << std::endl; 
        //     if(i2->getHost() == i->getHost() && i2->getPort() == i->getPort())
        //     {
        //         i->setFd(i2->getListenFd());
        //         serverDup = true;
        //     }
        // }
        // // which server you are sending to bind
        // if (!serverDup)
        std::cout << "Binding server with port" << i->getPort() << std::endl;
        i->bindServer();
        std::cout << "Server created" << std::endl;
    }
}

void ServerSocket::runServer()
{
    fd_set  read_cpy;
    fd_set  write_cpy;
    _biggest_fd = 0;
    int     selected;
    struct timeval timer;
    listenServer();
    std::cout << RED << "Server is running" << RESET << std::endl;
    while (true)
    {
        timer.tv_sec = 1;
        timer.tv_usec = 0;
        read_cpy = _read_fd;
        write_cpy = _write_fd;
        if ((selected = select(_biggest_fd + 1, &read_cpy, &write_cpy, NULL, &timer)) < 0)
        {
            std::cerr << "select error" << std::endl;
            exit (1);
        }
        for (int fd = 3; fd <= _biggest_fd; ++fd)
        {
            // std::cout << "_biggest_fd: " << _biggest_fd << std::endl;
            if (FD_ISSET(fd, &read_cpy) && _servers_map.count(fd)) {
                acceptNewConnection(_servers_map.find(fd)->second);
                std::cout << YELLOW << "New Connection" << RESET << std::endl;
            }
            else if (FD_ISSET(fd, &read_cpy) && _clients_map.count(fd))
                readRequest(fd, _clients_map[fd]);
            //need cgi part for response runing
        }
        checkTimeout();
    }
}

void ServerSocket::listenServer()
{
    FD_ZERO(&_read_fd); //+- need check
    FD_ZERO(&_write_fd);
    for (std::vector<ServerConfig>::iterator i = _servers.begin(); i != _servers.end(); ++i)
    {
        if (listen(i->getListenFd(), 420) == -1)
        {
            std::cerr << "listen error" << std::endl;
            exit(1);
        }
        if (fcntl(i->getListenFd(), F_SETFL, O_NONBLOCK) < 0)
        {
            std::cerr << "fcntl error" << std::endl;
            exit(1);
        }
        addToSet(i->getListenFd(), _read_fd);
        _servers_map.insert(std::make_pair(i->getListenFd(), *i)); //need to allows us to quickly look up the server associated with a file descriptor later on. 
    }
    _biggest_fd = _servers.back().getListenFd(); //need for 'select' func
}

void ServerSocket::acceptNewConnection(ServerConfig &serv)// we need it to  allows the server to maintain an up-to-date record of connected clients and their associated sockets for further communication and management.
{
    struct  sockaddr_in client_addr;
    long    client_addr_size = sizeof(client_addr);
    int     client_socket;
    Client  new_client(serv);
    if ((client_socket = accept(serv.getListenFd(), (struct sockaddr *)&client_addr, (socklen_t*)&client_addr_size))== -1)
    {
        std::cerr << "accept error" << std::endl;
        return ;
    }
    addToSet(client_socket, _read_fd);
    if (fcntl(client_socket, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << "fcntl error" << std::endl;
        removeFromSet(client_socket, _read_fd);
        close(client_socket);
        return ;
    }
    new_client.setSocket(client_socket);
    if (_clients_map.count(client_socket) != 0) // check if we already have this socket
        _clients_map.erase(client_socket);
    _clients_map.insert(std::make_pair(client_socket, new_client));
}

void ServerSocket::closeConnection(int fd)
{
    if (FD_ISSET(fd, &_write_fd))
        removeFromSet(fd, _write_fd);
    if (FD_ISSET(fd, &_read_fd))
        removeFromSet(fd, _read_fd);
    close(fd);
    _clients_map.erase(fd);
}

void ServerSocket::readRequest(const int &fd, Client &client)
{
    char    buf[MAX_CONTENT_LENGTH];
    int     readed = 0;
    readed = read(fd, buf, MAX_CONTENT_LENGTH);
    if (readed == 0)
    {
        std::cerr << "Closed Client Conection" << std::endl;
        closeConnection(fd);
        return ;
    }
    else if (readed < 0)
    {
        std::cerr << "fd read error" << std::endl;
        closeConnection(fd);
        return ;
    }
    else
    {
        client.request += buf;
        client.setTime();
        // client.response = HTTP::handleRequest(client.request, client.server);
        memset(buf, 0, sizeof(buf));
    }
    //check error code or if pars completed
}

// void ServerSocket::sendResponse(const int &fd, Client &client)
// {
//     int sent;
//     // std::string response = client.response.(); need get response header + body like entire response
    
// }

void ServerSocket::assignServer(Client &client)
{
    for(std::vector<ServerConfig>::iterator i = _servers.begin(); i != _servers.end(); ++i)
    {
        if (client.server.getHost() == i->getHost() && client.server.getPort() == i->getPort() /*&& client.request == i->getServerName()*/) //!
        {
            client.setServer(*i);
            return ;
        }
    }
}

void ServerSocket::checkTimeout()
{
    for(std::map<int, Client>::iterator i = _clients_map.begin(); i != _clients_map.end(); ++i)
    {
        if (time(NULL) - i->second.getLastTime() > 60)
        {
            std::cerr << "Client Timeout, Closing Conection" << std::endl;
            closeConnection(i->first);
            return ;
        }
    }
}

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
