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
    if (events & EPOLLIN)
        _serverSocket.readRequest(_clientSocket, this);
    if (events & EPOLLOUT)
        sendResponse();
    if (events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR))
        _serverSocket.closeConnection(_clientSocket);
}

void Client::sendResponse() {
    if (_responses.empty()) {
        _serverSocket.modifyEpoll(_clientSocket, EPOLLIN); // Modify epoll to listen for reading events only
        return;
    }
    std::string response = _responses.front();
    ssize_t bytesSent = send(_clientSocket, response.c_str(), response.size(), 0);
    if (bytesSent == -1)
        return;
    if (bytesSent == static_cast<ssize_t>(response.size()))
        _responses.pop_front();
    else
        _responses.front() = response.substr(bytesSent);
    if (_responses.empty()) {
        _serverSocket.removeFdFromMonitor(_clientSocket);
        close(_clientSocket);
        if (_cgiHandler != NULL)
            _cgiHandler->setState(CgiHandler::DONE);
    } else
        _serverSocket.modifyEpoll(_clientSocket, EPOLLOUT);
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
