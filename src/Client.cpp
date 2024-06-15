/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:10:23 by kilchenk          #+#    #+#             */
/*   Updated: 2024/06/15 17:30:52 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/* Constructors, destructors and ansignment operator*/
Client::Client(int epoll_fd) : _clientSocket(-1), _lastMsg(0), _epoll_fd(epoll_fd) {}
Client::Client(ServerConfig &serv, int epoll_fd) : _server(serv), _clientSocket(-1), _lastMsg(0), _epoll_fd(epoll_fd) {}
Client::~Client() {}

Client::Client(const Client &copy)
{
    if (this != &copy)
    {
        this->_clientAddress = copy._clientAddress;
        this->_lastMsg = copy._lastMsg;
        this->_clientSocket = copy._clientSocket;
        this->_server = copy._server;
        this->_requests = copy._requests;
        this->_responses = copy._responses;
        this->_incompleteRequest = copy._incompleteRequest;
    }
}

Client &Client::operator=(const Client &copy)
{
     if (this != &copy)
    {
        this->_clientAddress = copy._clientAddress;
        this->_lastMsg = copy._lastMsg;
        this->_clientSocket = copy._clientSocket;
        this->_server = copy._server;
        this->_requests = copy._requests;
        this->_responses = copy._responses;
        this->_incompleteRequest = copy._incompleteRequest;
    }
    return (*this);    
}

/* Geters */
const int 	        &Client::getSocket() const { return _clientSocket; }
const sockaddr_in	&Client::getAddress() const { return _clientAddress; }
const time_t	    &Client::getLastTime() const { return _lastMsg; }
const std::string	&Client::getIncompleteRequest() const { return _incompleteRequest; }
const ServerConfig	&Client::getServer() const { return _server; }
const int           &Client::getEpollFd() const { return _epoll_fd; }

/* Setters */
void	Client::setSocket(int &socket) { _clientSocket = socket; }
void	Client::setAddress(sockaddr_in &address) { _clientAddress = address; }
void	Client::setServer(ServerConfig &serv) { _server = serv; }
void	Client::setTime() { _lastMsg = time(NULL); }
void	Client::setIncompleteRequest(const std::string &req) { _incompleteRequest = req; }

/* Methods */
void	Client::addRequest(const std::string& req) { _requests.push(req); }
bool	Client::hasRequests() { return !_requests.empty(); }

std::string Client::getNextRequest() {
    if (_requests.empty()) return "";
    std::string req = _requests.front();
    _requests.pop();
    return req;
}

void	Client::addResponse(const std::string& resp) { _responses.push(resp); }

bool	Client::hasResponses() { return !_responses.empty(); }

std::string	&Client::getCurrentResponse() {
	if (_responses.empty()) return _emptyResponse;
	return _responses.front();
}
void	Client::removeCurrentResponse() {
	if (!_responses.empty())
        _responses.pop();
}

void	Client::clearClient() {
    // Clear all the data in the client object
    while (!_requests.empty()) _requests.pop();
    while (!_responses.empty()) _responses.pop();
    _incompleteRequest.clear();
}