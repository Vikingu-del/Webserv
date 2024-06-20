#include "CgiHandler.hpp"
#include "Logger.hpp"
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include "ServerSocket.hpp"

// Constructor
CgiHandler::CgiHandler(Client* client, const HTTP::Request& req, const std::string& cgiExt, const ServerConfig& server, const Location& location, ServerSocket* serverSocket)
    : _req(req), _client(client), _cgiExt(cgiExt), _server(server), _location(location), _resource(req.getResource()), _state(WRITING), _pid(0), _serverSocket(serverSocket) {
    _pipeIn[0] = -1;
    _pipeIn[1] = -1;
    _pipeOut[0] = -1;
    _pipeOut[1] = -1;
}

// Destructor
CgiHandler::~CgiHandler() {
    closePipes();
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
    _env["REMOTE_ADDR"] = utils::intToString(_server.getServerArddres().sin_addr.s_addr);
    _env["REMOTE_PORT"] = utils::intToString(ntohs(_server.getServerArddres().sin_port));
}

void CgiHandler::initCgi() {
    Logger::logMsg("INFO", CONSOLE_OUTPUT, "Initializing CGI");
    if (pipe(_pipeIn) == -1 || pipe(_pipeOut) == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "pipe error: %s", strerror(errno));
        return;
    }
    Logger::logMsg("INFO", CONSOLE_OUTPUT, "Pipes created successfully");
    _client->addFdToMonitor(_pipeOut[0], EPOLLIN);
    _serverSocket->getCgiPipeMap()[_pipeOut[0]] = this;  // Add to _cgiPipeMap

    _pid = fork();
    if (_pid == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "fork error: %s", strerror(errno));
        return;
    }
    if (_pid == 0) {
        // Child process
        Logger::logMsg("INFO", CONSOLE_OUTPUT, "Forked child process for CGI");
        close(_pipeIn[1]);
        close(_pipeOut[0]);
        dup2(_pipeIn[0], STDIN_FILENO);
        dup2(_pipeOut[1], STDOUT_FILENO);
        close(_pipeIn[0]);
        close(_pipeOut[1]);

        std::vector<char *> envp = createEnvArray();
        std::vector<char *> args;

        // Construct the full path to the script
        std::string script_path = _location.getRootLocation() + _resource;
        args.push_back(strdup(script_path.c_str()));
        args.push_back(NULL);
        execve(args[0], args.data(), envp.data());
        exit(1);
    } else {
        // Parent process
        Logger::logMsg("INFO", CONSOLE_OUTPUT, "Parent process continuing after fork");
        close(_pipeIn[0]);
        close(_pipeOut[1]);
    }
}


void CgiHandler::readCgiResponse() {
    char buffer[1024];
    ssize_t bytesRead;
    std::string completeResponse;

    while ((bytesRead = read(_pipeOut[0], buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        completeResponse += std::string(buffer, bytesRead);
    }
    if (bytesRead == -1 && errno != EAGAIN) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "read error: %s", strerror(errno));
    }

    int status;
    if (waitpid(_pid, &status, WNOHANG) == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "waitpid error: %s", strerror(errno));
    } else if (WIFEXITED(status)) {
        _exitStatus = WEXITSTATUS(status);
        Logger::logMsg(PURPLE, CONSOLE_OUTPUT, "Child process exited with status: %d", _exitStatus);
    } else if (WIFSIGNALED(status)) {
        Logger::logMsg(PURPLE, CONSOLE_OUTPUT, "Child process was terminated by signal: %d", WTERMSIG(status));
    }

    if (_exitStatus == 0) {
        if (completeResponse.find("HTTP/1.1") == std::string::npos) {
            std::string cgiResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + completeResponse;
            _client->addResponse(cgiResponse);
            std::cout << cgiResponse << std::endl;
        } else {
            _client->addResponse(completeResponse);
             Logger::logMsg(PURPLE, CONSOLE_OUTPUT, "Adding complete response: %s", completeResponse.c_str());
        }
    } else {
        _client->addResponse("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\nCGI script error");
    }

    Logger::logMsg(PURPLE, CONSOLE_OUTPUT, "execCgi completed with response: %s", completeResponse.c_str());
    _state = DONE; // Set the state to DONE after executing CGI
    (*_client).sendResponse();
}

void CgiHandler::sendCgiBody() {
    std::cout << YELLOW << "Sending CGI body" << RESET << std::endl;
    const std::string requestBody = _client->getIncompleteRequest();
    std::cout << YELLOW << "Sending CGI body: " << requestBody << RESET << std::endl;
    ssize_t bytesSent = write(_pipeIn[1], requestBody.c_str(), requestBody.size());
    if (bytesSent == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "write error: %s", strerror(errno));
    } else if (bytesSent < static_cast<ssize_t>(requestBody.size())) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "Partial write occurred. Expected: %zu, Sent: %zd", requestBody.size(), bytesSent);
    } else {
        setState(READING); // Move to reading state only if the write was successful
    }
}

void CgiHandler::closePipes() {
    Logger::logMsg("INFO", CONSOLE_OUTPUT, "Closing pipes");
    if (_pipeIn[0] != -1) close(_pipeIn[0]);
    if (_pipeIn[1] != -1) close(_pipeIn[1]);
    if (_pipeOut[0] != -1) close(_pipeOut[0]);
    if (_pipeOut[1] != -1) close(_pipeOut[1]);
}

std::vector<char *> CgiHandler::createEnvArray() {
    std::vector<char *> envp;
    for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); ++it) {
        std::string env_entry = it->first + "=" + it->second;
        char *env_cstr = new char[env_entry.size() + 1];
        std::strcpy(env_cstr, env_entry.c_str());
        envp.push_back(env_cstr);
    }
    envp.push_back(NULL); // Null-terminate the array
    return envp;
}