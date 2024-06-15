#include "CgiHandler.hpp"
#include <fcntl.h>
// #include <sys/types.h>
// #include <sys/wait.h>

CgiHandler::CgiHandler(const Client &client,
                    const HTTP::Request &req,
                    int epollFd,
                    std::string cgiExt,
                    const ServerConfig &server) : _req(req),
                                        _client(client),
                                        _cgiExt(cgiExt),
                                        _epollFd(epollFd),
                                        _server(server) {
    _resource = _req.getResource();
    initEnv();
    initCgi();
}

CgiHandler::~CgiHandler() {
    // closePipes();
}

CgiHandler &CgiHandler::operator=(const CgiHandler &copy) {
    if (this != &copy) {
        _env = copy._env;
        _server = copy._server;
        _req = copy._req;
        _res = copy._res;
        _client = copy._client;
        _pid = copy._pid;
        _cgiExt = copy._cgiExt;
        _resource = copy._resource;
        _pipeIn[0] = copy._pipeIn[0];
        _pipeIn[1] = copy._pipeIn[1];
        _pipeOut[0] = copy._pipeOut[0];
        _pipeOut[1] = copy._pipeOut[1];
        _epollFd = copy._epollFd;
        _readBytes = copy._readBytes;
        _readTime = copy._readTime;
        _statusCode = copy._statusCode;
        _isRead = copy._isRead;
        _isParsed = copy._isParsed;
        _length = copy._length;
        _exitStatus = copy._exitStatus;
    }
    return *this;
}

void    CgiHandler::initEnv() {
    _env["GATEWAY_INTERFACE"] = "CGI/1.1";
    _env["SERVER_PROTOCOL"] = "HTTP/1.1";
    _env["REQUEST_METHOD"] = HTTP::methodToString(_req.getMethod());
    _env["QUERY_STRING"] = _req.getQuery();
    _env["CONTENT_LENGTH"] = utils::intToString(_req.getBody().size());
    std::string contentType;
    _req.getHeader("Content-Type", contentType);
    _env["CONTENT_TYPE"] = contentType;
    _env["REQUEST_URI"] = _req.getUri();
    _env["SCRIPT_NAME"] = _resource.find('?') != std::string::npos ? _resource.substr(0, _resource.find('?')) : _resource;
    _env["SERVER_NAME"] = _server.getServerName();
    _env["SERVER_PORT"] = utils::intToString(_server.getPort());
    _env["REMOTE_ADDR"] = utils::intToString(_client.getAddress().sin_addr.s_addr);
    _env["REMOTE_PORT"] = utils::intToString(ntohs(_client.getAddress().sin_port));
}

void CgiHandler::initCgi() {
    if (pipe(_pipeIn) == -1 || pipe(_pipeOut) == -1) {
        perror("pipe error");
        return;
    }
    _pid = fork();
    if (_pid == -1) {
        perror("fork error");
        return;
    }
    if (_pid == 0) {
        // Child process
        close(_pipeIn[1]);
        close(_pipeOut[0]);
        dup2(_pipeIn[0], STDIN_FILENO);
        dup2(_pipeOut[1], STDOUT_FILENO);
        close(_pipeIn[0]);
        close(_pipeOut[1]);
        std::vector<char *> envp = createEnvArray();
        std::vector<char *> args;
        args.push_back(strdup(_resource.c_str()));
        args.push_back(NULL);
        execve(_cgiExt.c_str(), args.data(), envp.data());
        perror("execve error");
        exit(1);
    } else {
        // Parent process
        close(_pipeIn[0]);
        close(_pipeOut[1]);
        fcntl(_pipeOut[0], F_SETFL, O_NONBLOCK);
    }
}

std::vector<char *> CgiHandler::createEnvArray() {
    std::vector<char *> envp;
    for (std::map<std::string, std::string>::const_iterator it = _env.begin(); it != _env.end(); ++it) {
        std::string env = it->first + "=" + it->second;
        envp.push_back(strdup(env.c_str()));
    }
    envp.push_back(NULL);
    return envp;
}

void CgiHandler::execCgi() {
    // Implement I/O handling here
}

void CgiHandler::closePipes() {
    close(_pipeIn[0]);
    close(_pipeIn[1]);
    close(_pipeOut[0]);
    close(_pipeOut[1]);
}

void CgiHandler::deleteChild(int childFd) {
    // Implement child process cleanup
    
}

// Implement the rest of the getters and setters here

std::map<std::string, std::string> &CgiHandler::getEnv() { return _env; }
ServerConfig &CgiHandler::getConfig() { return _server; }
HTTP::Request &CgiHandler::getRequest() { return _req; }
HTTP::Response &CgiHandler::getResponse() { return _res; }
Client &CgiHandler::getClient() { return _client; }
pid_t CgiHandler::getPid() { return _pid; }
std::string &CgiHandler::getCgiExt() { return _cgiExt; }
std::string &CgiHandler::getResource() { return _resource; }
int CgiHandler::getPipeIn() { return _pipeIn[0]; }
int CgiHandler::getPipeOut() { return _pipeOut[0]; }
int CgiHandler::getEpollFd() { return _epollFd; }
int CgiHandler::getReadBytes() { return _readBytes; }
int CgiHandler::getReadTime() { return _readTime; }
int CgiHandler::getStatusCode() { return _statusCode; }
bool CgiHandler::getIsRead() { return _isRead; }
bool CgiHandler::getIsParsed() { return _isParsed; }
int CgiHandler::getLength() { return _length; }
int CgiHandler::getExitStatus() { return _exitStatus; }

void CgiHandler::setEnv(const std::map<std::string, std::string> &env) { _env = env; }
void CgiHandler::setConfig(const ServerConfig &server) { _server = server; }
void CgiHandler::setRequest(const HTTP::Request &req) { _req = req; }
void CgiHandler::setResponse(const HTTP::Response &res) { _res = res; }
void CgiHandler::setClient(const Client &client) { _client = client; }
void CgiHandler::setPid(pid_t pid) { _pid = pid; }
void CgiHandler::setCgiExt(const std::string &cgiExt) { _cgiExt = cgiExt; }
void CgiHandler::setResource(const std::string &resource) { _resource = resource; }
void CgiHandler::setPipeIn(int pipeIn) { _pipeIn[0] = pipeIn; }
void CgiHandler::setPipeOut(int pipeOut) { _pipeOut[0] = pipeOut; }
void CgiHandler::setEpollFd(int epollFd) { _epollFd = epollFd; }
void CgiHandler::setReadBytes(int readBytes) { _readBytes = readBytes; }
void CgiHandler::setReadTime(int readTime) { _readTime = readTime; }
void CgiHandler::setStatusCode(int statusCode) { _statusCode = statusCode; }
void CgiHandler::setIsRead(bool isRead) { _isRead = isRead; }
void CgiHandler::setIsParsed(bool isParsed) { _isParsed = isParsed; }
void CgiHandler::setLength(int length) { _length = length; }
void CgiHandler::setExitStatus(int exitStatus) { _exitStatus = exitStatus; }