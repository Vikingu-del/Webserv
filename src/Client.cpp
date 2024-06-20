#include "Client.hpp"
#include "ServerSocket.hpp"
#include "RequestHandler.hpp"
#include "CgiHandler.hpp"
#include "Logger.hpp"
#include <unistd.h>
#include <ctime>

Client::Client(ServerSocket& serverSocket) 
    : _lastMsg(time(NULL)), _incompleteRequest(""), _emptyResponse(""), _cgiHandler(NULL), _isCgiRequest(false), _serverSocket(serverSocket), _clientSocket(-1) {}

Client::Client(ServerConfig& serv, ServerSocket& serverSocket, int clientSocket) 
    : _lastMsg(time(NULL)), _incompleteRequest(""), _emptyResponse(""), _cgiHandler(NULL), _isCgiRequest(false), _serverSocket(serverSocket), _server(serv), _clientSocket(clientSocket) {}

Client::~Client() {
    if (_cgiHandler) {
        delete _cgiHandler;
    }
    close(_clientSocket);
}

void Client::handleEvent(int events) {
    Logger::logMsg("INFO", CONSOLE_OUTPUT, "Client handling events: %d", events);
    if (events & EPOLLIN) {
        Logger::logMsg("INFO", CONSOLE_OUTPUT, "Client EPOLLIN event for fd: %d", _clientSocket);
        _serverSocket.readRequest(_clientSocket, this);
    }
    if (events & EPOLLOUT) {
        Logger::logMsg("INFO", CONSOLE_OUTPUT, "Client EPOLLOUT event for fd: %d", _clientSocket);
        sendResponse();
    }
    if (events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
        Logger::logMsg("INFO", CONSOLE_OUTPUT, "Client EPOLLHUP, EPOLLRDHUP, or EPOLLERR event for fd: %d", _clientSocket);
        _serverSocket.closeConnection(_clientSocket);
    }
}

void Client::sendResponse() {
    if (_responses.empty()) {
        return;
    }
    std::string response = _responses.front();
    ssize_t bytesSent = send(_clientSocket, response.c_str(), response.size(), 0);
    if (bytesSent == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "send error: %s", strerror(errno));
        return;
    }
    if (bytesSent == static_cast<ssize_t>(response.size())) {
        _responses.pop_front();
    } else {
        _responses.front() = response.substr(bytesSent);
    }

    if (_responses.empty()) {
        // Mark the client as done and remove the client socket from the epoll monitoring list after sending all responses
        _cgiHandler->setState(CgiHandler::DONE);
        _serverSocket.removeFdFromMonitor(_clientSocket);
        close(_clientSocket); // Close the client socket after sending the response
    }
}

void Client::addResponse(const std::string& response) {
    _responses.push_back(response);
}

void Client::removeCurrentResponse() {
    if (!_responses.empty()) {
        _responses.pop_front();
    }
}

bool Client::hasResponses() const {
    return !_responses.empty();
}

std::string& Client::getCurrentResponse() {
    return _responses.front();
}

std::string Client::getIncompleteRequest() const {
    return _incompleteRequest;
}

void Client::setIncompleteRequest(const std::string& request) {
    _incompleteRequest = request;
}

void Client::setTime() {
    _lastMsg = time(NULL);
}

time_t Client::getLastTime() const {
    return _lastMsg;
}

void Client::setCgiRequest(bool isCgi) {
    _isCgiRequest = isCgi;
}

bool Client::isCgiRequest() const {
    return _isCgiRequest;
}

void Client::setCgiHandler(CgiHandler* handler) {
    _cgiHandler = handler;
}

CgiHandler* Client::getCgiHandler() const {
    return _cgiHandler;
}

void Client::addFdToMonitor(int fd, uint32_t events) {
    if (_monitoredFds.find(fd) == _monitoredFds.end()) {
        _serverSocket.addToEpoll(fd, uint32_t(events));
        _monitoredFds.insert(fd);
    }
}

int Client::getSocket() const {
    return _clientSocket;
}

ServerConfig Client::getServer() const {
    return _server;
}

bool Client::hasIncompleteRequest() const {
    return !_incompleteRequest.empty();
}

std::string Client::getNextRequest() {
    std::string request = _incompleteRequest;
    _incompleteRequest.clear();
    return request;
}
