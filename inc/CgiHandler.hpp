#pragma once

#include "Http.hpp"
#include "ServerConfig.hpp"
#include <sys/epoll.h>

class Client;  // Forward declaration

class CgiHandler {
public:
    enum CgiState { READING, WRITING, DONE };

private:
    std::map<std::string, std::string> _env;
    HTTP::Request _req;
    HTTP::Response _res;
    Client* _client;  // Use a pointer to Client
    pid_t _pid;
    std::string _cgiExt;
    std::string _resource;
    int _pipeIn[2];
    int _pipeOut[2];
    int _epollFd;
    ServerConfig _server;
    int _readBytes;
    int _readTime;
    int _statusCode;
    bool _isRead;
    bool _isParsed;
    int _length;
    int _exitStatus;
    CgiState _state;

    // Private Methods
    void initEnv();
    void closePipes();
    void deleteChild(int childFd);
    std::vector<char *> createEnvArray();

public:
    CgiHandler();
    CgiHandler(Client* client, const HTTP::Request &req, int epollFd, std::string cgiExt, const ServerConfig &server);
    ~CgiHandler();
    CgiHandler &operator=(const CgiHandler &copy);

    // Public Methods
    void initCgi();
    void execCgi();
    CgiState getState() const;
    void setState(CgiState state);
    int getPipeIn() const;
    int getPipeOut() const;
    void readCgiResponse(Client* client, CgiHandler* cgiHandler);
    void sendCgiBody(Client* client, CgiHandler* cgiHandler);
};
