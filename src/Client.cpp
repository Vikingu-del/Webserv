/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:10:23 by kilchenk          #+#    #+#             */
/*   Updated: 2024/05/16 18:41:46 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client()
{
    _last_msg = time(NULL);
}

Client::~Client()
{

}

Client::Client(const Client &copy)
{
    if (this != &copy)
    {
        this->_client_address = copy._client_address;
        this->_last_msg = copy._last_msg;
        this->_client_socket = copy._client_socket;
        this->server = copy.server;
    }
    return ;
}

Client &Client::operator=(const Client &copy)
{
    if (this != &copy)
    {
        this->_client_address = copy._client_address;
        this->_last_msg = copy._last_msg;
        this->_client_socket = copy._client_socket;
        this->server = copy.server;
    }
    return (*this);    
}

Client::Client(ServerConfig &server)
{
    setServer(server);
    _last_msg = time(NULL);
}

const int &Client::getSocket() const
{
    return (_client_socket);
}

const sockaddr_in &Client::getAddress() const
{
    return (_client_address);
}

const time_t &Client::getLastTime() const
{
    return (_last_msg);
}

// const HTTP::Request   &Client::getRequest() const
// {
//     return (request);
// }

void Client::setSocket(int &socket)
{
    _client_socket = socket;
}

void Client::setAddress(sockaddr_in &address)
{
    _client_address = address;
}

void Client::setServer(ServerConfig &serv)
{
    server = serv;
}

void Client::setTime()
{
    _last_msg = time(NULL);
}


void    Client::clearClient()
{
    // response.clear(); need func to clear all of it
    // request.clear(); 
}