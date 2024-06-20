#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "Http.hpp"
#include "Logger.hpp"
#include "ServerConfig.hpp"
#include "Client.hpp"
#include <string>
#include <map>
#include <sys/epoll.h>
#include <vector>

class Client;

class CgiHandler {
public:
    enum State {
        WRITING,
        READING,
        DONE
    };

private:
    const HTTP::Request& _req;
    Client* _client;
    std::string _cgiExt;
    // int _epollFd;
    ServerConfig _server;
    Location _location;
    std::string _resource;
    State _state;
    pid_t _pid;
    int _pipeIn[2];
    int _pipeOut[2];
    std::map<std::string, std::string> _env;
    ServerSocket* _serverSocket;  // Add ServerSocket pointer

    void initEnv();
    std::vector<char*> createEnvArray();
    void closePipes();

public:
     CgiHandler(Client* client, const HTTP::Request& req, const std::string& cgiExt, const ServerConfig& server, const Location& location, ServerSocket* serverSocket);
    ~CgiHandler();

    void initCgi();
    void readCgiResponse();
    void sendCgiBody();
    State getState() const { return _state; }
    void setState(State state) { _state = state; }

    // getters
    int getPipeIn() const { return _pipeIn[1]; }
    int getPipeOut() const { return _pipeOut[0]; }

private:
    int _exitStatus;  // Declare _exitStatus here
};

#endif // CGI_HANDLER_HPP
