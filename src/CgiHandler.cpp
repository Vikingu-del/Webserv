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
    _env["PATH_INFO"] = _resource;
}


void CgiHandler::initCgi() {
    std::string file = _resource.find_last_of('.') != std::string::npos ? _resource.substr(_resource.find_last_of('/') + 1) :  _location.getIndexLocation();
    std::string cgiScriptPath = _location.getRootLocation() + "/cgi-bin/" + file;
    _resource = cgiScriptPath;
    if (!cgiScriptExists(cgiScriptPath)) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "CGI script not found: %s", cgiScriptPath.c_str());
        sendErrorResponse(404, "Not Found", "CGI script not found");
        return;
    }
    if (pipe(_pipeIn) == -1 || pipe(_pipeOut) == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "pipe error: %s", strerror(errno));
        return;
    }
    _client->addFdToMonitor(_pipeOut[0], EPOLLIN);
    _serverSocket->getCgiPipeMap()[_pipeOut[0]] = this;  // Add to _cgiPipeMap
    initEnv();
     std::vector<char *> envp = createEnvArray();
    std::vector<char *> args;

    // Construct the full path to the script
    args.push_back(strdup(cgiScriptPath.c_str()));
    args.push_back(0);
    for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); ++it) {
        Logger::logMsg("INFO", CONSOLE_OUTPUT, "env: %s=%s", it->first.c_str(), it->second.c_str());
    }
    _pid = fork();
    if (_pid == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "fork error: %s", strerror(errno));
        sendErrorResponse(500, "Internal Server Error", "CGI script error");
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
        execve(args[0], &args[0], &envp[0]);
        perror("execve");
        exit(1);
    } else {
        // Parent process
        Logger::logMsg("INFO", CONSOLE_OUTPUT, "Parent process continuing after fork");
        close(_pipeIn[0]);
        close(_pipeOut[1]);
        setState(WRITING); // Set the state to WRITING after forking
        _serverSocket->modifyEpoll(_pipeOut[0], EPOLLIN); // Update epoll to listen for reading events
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
        std::string cgiResponse;
        if (completeResponse.find("HTTP/1.1") == std::string::npos) {
            cgiResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + completeResponse;
        } else {
            cgiResponse = completeResponse;
        }
        _client->addResponse(cgiResponse);
    } else {
        _client->addResponse("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\nCGI script error");
    }

    Logger::logMsg(PURPLE, CONSOLE_OUTPUT, "execCgi completed with response: %s", completeResponse.c_str());
    _state = DONE; // Set the state to DONE after executing CGI
}

void CgiHandler::sendCgiBody() {
    const std::string requestBody = _req.getBody();
    ssize_t bytesSent = write(_pipeIn[1], requestBody.c_str(), requestBody.size());
    if (bytesSent == -1) {
        Logger::logMsg(RED, CONSOLE_OUTPUT, "write error: %s", strerror(errno));
        close(_pipeIn[1]);
        return;
    }
    close(_pipeIn[1]);
    setState(READING);
    _serverSocket->modifyEpoll(_pipeOut[0], EPOLLIN);
}

void CgiHandler::closePipes() {
    Logger::logMsg("INFO", CONSOLE_OUTPUT, "Closing pipes");
    if (_pipeIn[0] != -1) close(_pipeIn[0]);
    if (_pipeIn[1] != -1) close(_pipeIn[1]);
    if (_pipeOut[0] != -1) close(_pipeOut[0]);
    if (_pipeOut[1] != -1) close(_pipeOut[1]);
}

std::vector<char*> CgiHandler::createEnvArray() {
    std::vector<char*> envp;
    for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); ++it) {
        std::string env_entry = it->first + "=" + it->second;
        char* env_cstr = new char[env_entry.size() + 1];
        std::strcpy(env_cstr, env_entry.c_str());
        envp.push_back(env_cstr);
    }
    envp.push_back(0); // Null-terminate the array
    return envp;
}

bool CgiHandler::cgiScriptExists(const std::string& scriptPath) {
    if (ConfigFile::checkFileExistence(scriptPath) == 1) {
        return true;
    } else return false;
}

void CgiHandler::sendErrorResponse(int statusCode, const std::string& statusText, const std::string& message) {
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n"
                   << "Content-Type: text/html\r\n"
                   << "Content-Length: " << message.size() << "\r\n"
                   << "\r\n"
                   << message;

    std::string response = responseStream.str();
    _client->addResponse(response);
    _serverSocket->modifyEpoll(_client->getSocket(), EPOLLOUT);
    setState(DONE);
}

std::string CgiHandler::handleGetResponse(const std::string& response) {
    Logger::logMsg("INFO", CONSOLE_OUTPUT, "Handling GET response");
    if (response.find("HTTP/1.1") == std::string::npos) {
        return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + response;
    }
    return response;
}

std::string CgiHandler::handlePostResponse(const std::string& response) {
    Logger::logMsg("INFO", CONSOLE_OUTPUT, "Handling POST response");
    if (response.find("HTTP/1.1") == std::string::npos) {
        return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + response;
    }
    return response;
}

std::string CgiHandler::handleDeleteResponse(const std::string& response) {
    Logger::logMsg("INFO", CONSOLE_OUTPUT, "Handling DELETE response");
    if (response.find("HTTP/1.1") == std::string::npos) {
        return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + response;
    }
    return response;
}