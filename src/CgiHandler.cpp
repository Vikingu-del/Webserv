#include "CgiHandler.hpp"
#include <fcntl.h>
#include <sys/wait.h>
#include "Client.hpp"
#include "Logger.hpp"

CgiHandler::CgiHandler() : _pid(0), _readBytes(0), _readTime(0), _statusCode(0), _isRead(false), _isParsed(false), _length(0), _exitStatus(0), _state(WRITING) {}
CgiHandler::CgiHandler(Client* client, const HTTP::Request &req, int epollFd, std::string cgiExt, const ServerConfig &server)
    : _req(req), _client(client), _cgiExt(cgiExt), _epollFd(epollFd), _server(server), _state(WRITING) {
    _resource = _req.getResource();
    initEnv();
    initCgi();
}

CgiHandler::~CgiHandler() {
    closePipes();
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
        _state = copy._state;
    }
    return *this;
}

void CgiHandler::initEnv() {
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
    _env["REMOTE_ADDR"] = utils::intToString(_client->getAddress().sin_addr.s_addr);
    _env["REMOTE_PORT"] = utils::intToString(ntohs(_client->getAddress().sin_port));
}

void CgiHandler::initCgi() {
    if (pipe(_pipeIn) == -1 || pipe(_pipeOut) == -1) {
        Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "pipe error");
        return;
    }
    _pid = fork();
    if (_pid == -1) {
        Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "fork error");
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
        Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "execve error");
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
    char buffer[1024];
    ssize_t bytesRead;

    // Read from CGI script's stdout
    while ((bytesRead = read(_pipeOut[0], buffer, 1023)) > 0) {
        buffer[bytesRead] = '\0';
        _res.appendToBody(std::string(buffer, bytesRead));
    }
    if (bytesRead == -1 && errno != EAGAIN) {
        Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "read error: %s", strerror(errno));
    }
    int status;
    if (waitpid(_pid, &status, WNOHANG) == -1) {
        Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "waitpid error: %s", strerror(errno));
    }
    if (WIFEXITED(status)) {
        _exitStatus = WEXITSTATUS(status);
    }
    if (WIFEXITED(status)) {
        Logger::logMsg("INFO", CONSOLE_OUTPUT, "Child process exited with status: %d", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        Logger::logMsg("INFO", CONSOLE_OUTPUT, "Child process was terminated by signal: %d", WTERMSIG(status));
    }
}

void CgiHandler::closePipes() {
    close(_pipeIn[0]);
    close(_pipeIn[1]);
    close(_pipeOut[0]);
    close(_pipeOut[1]);
}

void CgiHandler::deleteChild(int childFd) {
    close(_pipeIn[1]);
    close(_pipeOut[0]);

    // Remove the file descriptor from the epoll instance
    if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, childFd, NULL) == -1) {
        Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "epoll_ctl DEL error: %s", strerror(errno));
    }

    // Wait for the child process to exit
    int status;
    if (waitpid(_pid, &status, 0) == -1) {
        Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "waitpid error: %s", strerror(errno));
    }

    if (WIFEXITED(status)) {
        Logger::logMsg("INFO", CONSOLE_OUTPUT, "Child process exited with status: %d", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        Logger::logMsg("INFO", CONSOLE_OUTPUT, "Child process killed by signal: %d", WTERMSIG(status));
    }
}

CgiHandler::CgiState CgiHandler::getState() const {
    return _state;
}

void CgiHandler::setState(CgiState state) {
    _state = state;
}

int CgiHandler::getPipeIn() const {
    return _pipeIn[1];
}

int CgiHandler::getPipeOut() const {
    return _pipeOut[0];
}

void CgiHandler::readCgiResponse(Client* client, CgiHandler* cgiHandler) {
    char buffer[1024];
    ssize_t bytesRead;
    bool readErrorOccurred = false;
    std::string completeResponse;

    while ((bytesRead = read(cgiHandler->_pipeOut[0], buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        completeResponse += std::string(buffer, bytesRead);
    }
    if (!completeResponse.empty()) {
        client->addResponse(completeResponse);
    }
    if (bytesRead == -1 && errno != EAGAIN) {
        Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "read error: %s", strerror(errno));
        readErrorOccurred = true;
    }

    int status;
    if (waitpid(cgiHandler->_pid, &status, WNOHANG) == -1) {
        Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "waitpid error: %s", strerror(errno));
    } else if (WIFEXITED(status)) {
        cgiHandler->_exitStatus = WEXITSTATUS(status);
        Logger::logMsg("INFO", CONSOLE_OUTPUT, "Child process exited with status: %d", cgiHandler->_exitStatus);
        if (!readErrorOccurred) {
            cgiHandler->setState(DONE);
        }
    } else if (WIFSIGNALED(status)) {
        Logger::logMsg("INFO", CONSOLE_OUTPUT, "Child process was terminated by signal: %d", WTERMSIG(status));
    }
}

void CgiHandler::sendCgiBody(Client* client, CgiHandler* cgiHandler) {
    if (client->hasRequests()) {
        const std::string requestBody = client->getNextRequest(); // This now retrieves and removes the request
        ssize_t bytesSent = write(cgiHandler->_pipeIn[1], requestBody.c_str(), requestBody.size());
        if (bytesSent == -1) {
            Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "write error: %s", strerror(errno));
        } else if (bytesSent < static_cast<ssize_t>(requestBody.size())) {
            Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "Partial write occurred. Expected: %zu, Sent: %zd", requestBody.size(), bytesSent);
        // Handle partial write or retry logic here
        } else {
            cgiHandler->setState(READING); // Move to reading state only if the write was successful
        }
    }
}