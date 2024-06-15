# CGI Support Implementation

This code snippet demonstrates the implementation of CGI (Common Gateway Interface) support in a web server. CGI allows the server to execute external programs or scripts and generate dynamic content for the HTTP response.

## Steps to Implement CGI Support

To implement CGI support in your web server, you'll need to follow these steps:

1. Identify CGI Requests: Detect requests that should be handled by CGI based on file extension or directory.
2. Set Up Environment Variables: Prepare the necessary environment variables for the CGI script.
3. Execute the CGI Script: Run the CGI script and capture its output.
4. Send the Output Back to the Client: Return the output of the CGI script as the HTTP response.

## Sample Implementation

The provided code consists of three classes: `ServerConfig`, `RequestHandler`, and `HTTP`. Here's a brief overview of each class:

### ServerConfig

The `ServerConfig` class represents the configuration settings for the web server. It includes methods to set and get CGI extensions, root directory, and error pages.

### RequestHandler

The `RequestHandler` class handles incoming HTTP requests and generates appropriate responses. It includes methods to handle CGI requests, read files, handle autoindex, handle errors, and execute CGI scripts.

### HTTP

The `HTTP` class provides constants and utility functions related to the HTTP protocol.

Please note that this code snippet is a partial implementation and may require additional code to fully integrate CGI support into your web server.

For more details on CGI implementation, please refer to the provided file path: `/home/eseferi/Desktop/git_projects/Webserv/cgi.md`.
To implement CGI support in your web server, you'll need to follow these steps:

    Identify CGI Requests: Detect requests that should be handled by CGI based on file extension or directory.
    Set Up Environment Variables: Prepare the necessary environment variables for the CGI script.
    Execute the CGI Script: Run the CGI script and capture its output.
    Send the Output Back to the Client: Return the output of the CGI script as the HTTP response.

Here's a step-by-step breakdown of the process:
1. Identify CGI Requests

You'll need to modify your request handling logic to detect when a request should be processed by a CGI script. This can be based on the file extension or directory.
2. Set Up Environment Variables

CGI scripts rely on various environment variables to function correctly. These include QUERY_STRING, REQUEST_METHOD, CONTENT_TYPE, CONTENT_LENGTH, PATH_INFO, and more.
3. Execute the CGI Script

Use fork() and execve() to run the CGI script in a child process, capturing its output.
4. Send the Output Back to the Client

Read the output from the CGI script and send it back to the client as the HTTP response.

Here's a sample implementation:
ServerConfig.hpp

First, ensure your ServerConfig class can specify which file extensions should be handled by CGI:


#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <string>
#include <vector>
#include <map>

class ServerConfig {
private:
    std::vector<std::string> _cgiExtensions;
    std::map<short, std::string> _errorPages;
    std::string _root;

public:
    ServerConfig();
    ~ServerConfig();

    void setCgiExtensions(const std::vector<std::string> &extensions);
    const std::vector<std::string>& getCgiExtensions() const;
    void setRoot(const std::string &root);
    const std::string& getRoot() const;
    void setErrorPages(const std::map<short, std::string> &errorPages);
    const std::map<short, std::string>& getErrorPages() const;
};

#endif


ServerConfig.cpp

#include "ServerConfig.hpp"

ServerConfig::ServerConfig() {}

ServerConfig::~ServerConfig() {}

void ServerConfig::setCgiExtensions(const std::vector<std::string> &extensions) {
    _cgiExtensions = extensions;
}

const std::vector<std::string>& ServerConfig::getCgiExtensions() const {
    return _cgiExtensions;
}

void ServerConfig::setRoot(const std::string &root) {
    _root = root;
}

const std::string& ServerConfig::getRoot() const {
    return _root;
}

void ServerConfig::setErrorPages(const std::map<short, std::string> &errorPages) {
    _errorPages = errorPages;
}

const std::map<short, std::string>& ServerConfig::getErrorPages() const {
    return _errorPages;
}


RequestHandler.hpp

Add methods for handling CGI requests.

#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include <string>
#include <map>
#include <vector>
#include "HTTP.hpp"
#include "ServerConfig.hpp"

class RequestHandler {
private:
    ServerConfig _server;
    HTTP::Request _request;
    HTTP::Response _response;
    std::map<short, std::string> _errorPages;

    std::pair<std::string, std::string> readFile(const std::string &path);
    void handleAutoindex(std::pair<std::string, std::string> &responseBody, std::map<std::string, HTTP::Header> &responseHeaders, const std::string &directoryPath);
    void handleError(std::pair<std::string, std::string> &responseBody, const std::string &errorPagePath);
    void executeCgi(const std::string &filePath, const std::string &scriptName);

public:
    RequestHandler();
    RequestHandler(const ServerConfig &server, const std::string &request);

    const ServerConfig &getServer() const;
    const HTTP::Request &getRequest() const;
    const HTTP::Response &getResponse() const;

    void setServer(const ServerConfig &server);
    void setRequest(const HTTP::Request &request);
    void setResponse(const HTTP::Response &response);

    void handleRequest();
    void handleGetRequest(std::pair<std::string, std::string> &responseBody, std::map<std::string, HTTP::Header> &responseHeaders, std::vector<Location>::const_iterator &i, std::string &resource, std::string &directoryPath);
};

#endif


RequestHandler.cpp

Implement CGI execution logic.

#include "RequestHandler.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <cstring>
#include <cstdlib>

// Global file cache
std::map<std::string, std::string> fileCache;

std::pair<std::string, std::string> RequestHandler::readFile(const std::string &path) {
    std::string extension = path.substr(path.find_last_of(".") + 1);
    if (fileCache.find(path) != fileCache.end()) {
        std::cout << "File found in cache" << std::endl;
        return std::make_pair(fileCache[path], extension);
    }
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        // Return the content of the 404 error page or a default error message directly
        std::string errorPage = "404 not found"; // Default error message
        if (_errorPages.find(404) != _errorPages.end()) {
            std::cout << "Error page found: " << _server.getRoot() + _errorPages[404] << std::endl;
            std::ifstream errorFile((_server.getRoot() + _errorPages[404]).c_str(), std::ios::binary);
            if (errorFile.is_open()) {
                std::stringstream buffer;
                buffer << errorFile.rdbuf();
                errorPage = buffer.str();
            }
        }
        return std::make_pair(errorPage, "html");
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    fileCache[path] = buffer.str();
    return std::make_pair(fileCache[path], extension);
}

void RequestHandler::handleAutoindex(std::pair<std::string, std::string> &responseBody, std::map<std::string, HTTP::Header> &responseHeaders, const std::string &directoryPath) {
    // Autoindex handling logic
    std::cout << "Generating autoindex for directory: " << directoryPath << std::endl;
    responseBody.first = "<html><body><h1>Index of " + directoryPath + "</h1></body></html>"; // Simplified example
    responseBody.second = "html";
    responseHeaders["Content-Type"] = HTTP::Header("Content-Type", "text/html");
}

void RequestHandler::handleError(std::pair<std::string, std::string> &responseBody, const std::string &errorPagePath) {
    responseBody = readFile(errorPagePath);
}

void RequestHandler::executeCgi(const std::string &filePath, const std::string &scriptName) {
    pid_t pid;
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        // Child process
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        char *argv[] = {strdup(scriptName.c_str()), strdup(filePath.c_str()), NULL};
        char *envp[] = {NULL};

        execve(scriptName.c_str(), argv, envp);
        perror("execve");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        close(pipefd[1]);

        char buffer[1024];
        std::string result;
        ssize_t count;
        while ((count = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            result.append(buffer, count);
        }
        close(pipefd[0]);
        waitpid(pid, NULL, 0);

        _response = HTTP::Response(HTTP::OK, HTTP::HTTP_1_1, std::map<std::string, HTTP::Header>(), result);
    }
}

void RequestHandler::handleGetRequest(std::pair<std::string, std::string> &responseBody,
                                    std::map<std::string, HTTP::Header> &responseHeaders,
                                    std::vector<Location>::const_iterator &i,
                                    std::string &resource,
                                    std::string &directoryPath) {
    responseBody.second = _server.getMimeType(i->getType());
    std::cout << "Directory: " << directoryPath << std::endl;
    std::cout << "Resource: " << resource << std::endl;
    std::cout << "Index location: " << i->getIndexLocation() << std::endl;
    std::cout << "Root location: " << i->getRootLocation() << std::endl;
    std::string filePath = i->getRootLocation() + (directoryPath

void RequestHandler::handleGetRequest(std::pair<std::string, std::string> &responseBody,
                                    std::map<std::string, HTTP::Header> &responseHeaders,
                                    std::vector<Location>::const_iterator &i,
                                    std::string &resource,
                                    std::string &directoryPath) {
    responseBody.second = _server.getMimeType(i->getType());
    std::cout << "Directory: " << directoryPath << std::endl;
    std::cout << "Resource: " << resource << std::endl;
    std::cout << "Index location: " << i->getIndexLocation() << std::endl;
    std::cout << "Root location: " << i->getRootLocation() << std::endl;
    std::string filePath = i->getRootLocation() + (directoryPath == resource ? i->getIndexLocation() : resource);
    std::cout << "File path: " << filePath << std::endl;

    // Use a logging library instead of std::cout for production
    responseBody = readFile(filePath);
    if (responseBody.first == "404 not found") {
        if (i->getAutoindex()) {
            std::cout << "Autoindex enabled: " << i->getRootLocation() + directoryPath << std::endl;
            handleAutoindex(responseBody, responseHeaders, i->getRootLocation() + directoryPath);
            return;
        } else {
            std::map<short, std::string>::const_iterator it = _errorPages.find(404);
            if (it != _errorPages.end() && !it->second.empty()) {
                handleError(responseBody, it->second);
                responseBody.second = "text/html";
                responseHeaders["Content-Type"] = HTTP::Header("Content-Type", "text/html");
                _response = HTTP::Response(HTTP::NOT_FOUND, HTTP::HTTP_1_1, responseHeaders, responseBody.first);
                return;
            } else {
                responseBody.first = "Error: 404 Not Found";
                responseBody.second = "text/html";
            }
        }
    }

    // Check if the request should be handled by CGI
    std::string extension = filePath.substr(filePath.find_last_of(".") + 1);
    if (std::find(_server.getCgiExtensions().begin(), _server.getCgiExtensions().end(), extension) != _server.getCgiExtensions().end()) {
        executeCgi(filePath, filePath);
        return;
    }

    responseHeaders["Content-Type"] = HTTP::Header("Content-Type", responseBody.second);
    responseHeaders["Connection"] = HTTP::Header("Connection", "keep-alive");
    _response = HTTP::Response(HTTP::OK, HTTP::HTTP_1_1, responseHeaders, responseBody.first);
}

RequestHandler::RequestHandler() : _server(), _request(), _response() {}

RequestHandler::RequestHandler(const ServerConfig &server, const std::string &request) 
    : _server(server), _request(HTTP::Request::deserialize(request)), _response() {}

const ServerConfig &RequestHandler::getServer() const {
    return this->_server;
}

const HTTP::Request &RequestHandler::getRequest() const {
    return this->_request;
}

const HTTP::Response &RequestHandler::getResponse() const {
    return this->_response;
}

void RequestHandler::setServer(const ServerConfig &server) {
    this->_server = server;
}

void RequestHandler::setRequest(const HTTP::Request &request) {
    this->_request = request;
}

void RequestHandler::setResponse(const HTTP::Response &response) {
    this->_response = response;
}

void RequestHandler::handleRequest() {
    try {
        switch (this->_request.getMethod()) {
            case HTTP::GET:
                std::pair<std::string, std::string> responseBody;
                std::map<std::string, HTTP::Header> responseHeaders;
                std::vector<Location>::const_iterator i; // You need to determine the correct location iterator
                std::string resource = _request.getResource();
                std::string directoryPath = ""; // You need to set the correct directory path based on the resource
                handleGetRequest(responseBody, responseHeaders, i, resource, directoryPath);
                break;
            default:
                handleBadRequest();
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        handleBadRequest();
    }
}

void RequestHandler::handleBadRequest() {
    this->_response = HTTP::Response(HTTP::BAD_REQUEST, HTTP::HTTP_1_1, std::map<std::string, HTTP::Header>(), "Bad request");
}

Summary

    Identify CGI Requests: Check the file extension to determine if the request should be handled by CGI.
    Set Up Environment Variables: Prepare necessary environment variables for the CGI script.
    Execute the CGI Script: Use fork() and execve() to run the CGI script in a child process, capturing its output.
    Send the Output Back to the Client: Return the output of the CGI script as the HTTP response.

This implementation should cover the basic functionality needed to support CGI scripts in your web server. Adjustments may be necessary based on specific requirements and server configuration.