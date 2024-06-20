#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ServerConfig.hpp"
#include <string>
#include <deque>
#include <set>

class ServerSocket;
class CgiHandler;

class Client {
private:
    time_t _lastMsg;
    std::string _incompleteRequest;
    std::string _emptyResponse;
    CgiHandler* _cgiHandler;
    bool _isCgiRequest;
    ServerSocket& _serverSocket;
    ServerConfig _server;
    std::deque<std::string> _responses;
    std::set<int> _monitoredFds;
    int _clientSocket;

public:
    Client(ServerSocket& serverSocket);
    Client(ServerConfig& serv, ServerSocket& serverSocket, int clientSocket);
    ~Client();

    void handleEvent(int events);
    void sendResponse();
    void addResponse(const std::string& response);
    void removeCurrentResponse();
    bool hasResponses() const;
    std::string& getCurrentResponse();
    std::string getIncompleteRequest() const;
    void setIncompleteRequest(const std::string& request);
    void setTime();
    time_t getLastTime() const;
    void setCgiRequest(bool isCgi);
    bool isCgiRequest() const;
    void setCgiHandler(CgiHandler* handler);
    CgiHandler* getCgiHandler() const;
    void addFdToMonitor(int fd, uint32_t events);
    int getSocket() const;
    ServerConfig getServer() const;
    bool hasIncompleteRequest() const;
    std::string getNextRequest();
};

#endif // CLIENT_HPP
