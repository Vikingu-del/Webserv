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
    HTTP::Response _res;
    Client* _client;
    std::string _cgiExt;
    ServerConfig _server;
    Location _location;
    std::string _resource;
    State _state;
    pid_t _pid;
    int _pipeIn[2];
    int _pipeOut[2];
    std::map<std::string, std::string> _env;
    ServerSocket* _serverSocket;
    int _exitStatus;

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
    Client* getClient() const { return _client; }

    bool cgiScriptExists(const std::string& scriptPath);
    void sendErrorResponse(int statusCode, const std::string& statusText, const std::string& message);
    std::string handleGetResponse(const std::string& response);
    std::string handlePostResponse(const std::string& response);
    std::string handleDeleteResponse(const std::string& response);
    void handleAutoindex(std::string &responseBody, std::map<std::string, HTTP::Header> &responseHeaders, const std::string &directoryPath);
};

#endif // CGI_HANDLER_HPP