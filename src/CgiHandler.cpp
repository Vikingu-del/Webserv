#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::CgiHandler(const Client &client, const HTTP::Request &req, int epollFd, std::string cgiExt) : _client(client), _req(req), _epollFd(epollFd), _cgiExt(cgiExt) {
    _pipeIn[0] = -1;
    _pipeIn[1] = -1;
    _pipeOut[0] = -1;
    _pipeOut[1] = -1;
    _readBytes = 0;
    _readTime = 0;
    _statusCode = 0;
    _isRead = false;
    _isParsed = false;
    _exitStatus = 0;
    initEnv();
    
}