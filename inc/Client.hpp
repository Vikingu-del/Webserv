#pragma once

#include <netinet/in.h>
#include "ConfigFileParser.hpp"
#include "ServerConfig.hpp"
#include <queue>
#include "defines.h"

class CgiHandler;  // Forward declaration

class Client
{
private:
    ServerConfig _server;
    int _clientSocket;
    struct sockaddr_in _clientAddress;
    time_t _lastMsg;
    std::string _incompleteRequest;
    std::queue<std::string> _requests;
    std::queue<std::string> _responses;
    std::string _emptyResponse;
    int _epoll_fd;
    CgiHandler* _cgiHandler;  // Use a pointer to CgiHandler
    bool _isCgiRequest;  // New flag to indicate if the request is CGI

public:
    /* ORTODOX */
    Client();
    Client(int epoll_fd);
    Client(const Client &copy);
    Client(ServerConfig &serv, int epoll_fd);
    Client &operator=(const Client &copy);
    ~Client();
    /* Getters */
    const int &getSocket() const;
    const struct sockaddr_in &getAddress() const;
    const time_t &getLastTime() const;
    const ServerConfig &getServer() const;
    const std::string &getIncompleteRequest() const;
    const int &getEpollFd() const;
    /* Setters */
    void setSocket(int &socket);
    void setAddress(sockaddr_in &address);
    void setServer(ServerConfig &serv);
    void setTime(); // update time
    void setIncompleteRequest(const std::string &req);
    /* Methods */
    void addRequest(const std::string& req);
    bool hasRequests();
    std::string getNextRequest();
    void addResponse(const std::string& resp);
    bool hasResponses();
    std::string &getCurrentResponse();
    void removeCurrentResponse();
    void clearClient();
    bool isCgiRequest() const { return _isCgiRequest; }
    void setCgiRequest(bool isCgi) { _isCgiRequest = isCgi; }
    CgiHandler* getCgiHandler() { return _cgiHandler; }
    void setCgiHandler(CgiHandler* handler) { _cgiHandler = handler; }
};
