#include "RequestHandler.hpp"
#include <algorithm>
#include "CgiHandler.hpp"
#include "Logger.hpp"

std::map<std::string, std::string> RequestHandler::fileCache;

RequestHandler::RequestHandler() : _server(), _request(), _response(), _errorPages(), _showAutoIndex(0), _client(0) {}

RequestHandler::RequestHandler(const ServerConfig &server, const std::string &request, const Client *client)
    : _server(server), _request(HTTP::Request::deserialize(request)), _response(), _errorPages(server.getErrorPages()), _showAutoIndex(0) {
    _client = const_cast<Client *>(client);
}

const ServerConfig &RequestHandler::getServer() const {
    return _server;
}

const HTTP::Request &RequestHandler::getRequest() const {
    return _request;
}

const HTTP::Response &RequestHandler::getResponse() const {
    return _response;
}

const Client *RequestHandler::getClient() const {
    return _client;
}

void RequestHandler::setErrorPages(const std::map<short, std::string> &errorPages) {
    _errorPages = errorPages;
}

void RequestHandler::setServer(const ServerConfig &server) {
    _server = server;
}

void RequestHandler::setRequest(const HTTP::Request &request) {
    _request = request;
}

void RequestHandler::setResponse(const HTTP::Response &response) {
    _response = response;
}

void RequestHandler::setClient(Client *client) {
    _client = client;
}

std::pair<std::string, std::string> RequestHandler::readFile(const std::string &path) {
    std::string extension = path.substr(path.find_last_of(".") + 1);
    if (fileCache.find(path) != fileCache.end()) {
        Logger::logMsg("INFO", CONSOLE_OUTPUT, "File found in cache: %s", path.c_str());
        return std::make_pair(fileCache[path], extension);
    }
    if (_showAutoIndex == 1) _showAutoIndex = 0;
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open()) {
        Logger::logMsg("ERROR", CONSOLE_OUTPUT, "Failed to open file: %s", path.c_str());
        std::string errorPage = "404 not found"; // Default error message
        if (_errorPages.find(404) != _errorPages.end()) {
            Logger::logMsg("INFO", CONSOLE_OUTPUT, "Error page found: %s", (_server.getRoot() + _errorPages[404]).c_str());
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

void RequestHandler::handleGetRequest(std::pair<std::string, std::string> &responseBody,
                                      std::map<std::string, HTTP::Header> &responseHeaders,
                                      std::vector<Location>::const_iterator &i,
                                      std::string &resource,
                                      std::string &directoryPath) {
    Logger::logMsg("INFO", CONSOLE_OUTPUT, "Handling GET request");
    
    // Determine the MIME type
    responseBody.second = _server.getMimeType(i->getType());

    // Construct the file path
    std::string filePath = i->getRootLocation() + (directoryPath == resource ? i->getIndexLocation() : resource);
    Logger::logMsg("INFO", CONSOLE_OUTPUT, "File path: %s", filePath.c_str());

    // Check if autoindex is enabled
    _showAutoIndex = i->getAutoindex();

    // Read the file
    responseBody = readFile(filePath);

    // Handle file not found case
    if (responseBody.first == "404 not found" || (responseBody.first == readFile(_errorPages[404]).first)) {
        if (i->getAutoindex() && _showAutoIndex == 0) {
            Logger::logMsg("INFO", CONSOLE_OUTPUT, "Autoindex enabled: %s", (i->getRootLocation() + directoryPath).c_str());
            handleAutoindex(responseBody, responseHeaders, i->getRootLocation() + directoryPath);
            return;
        } else {
            std::map<short, std::string>::const_iterator it = _errorPages.find(404);
            if (it != _errorPages.end() && !it->second.empty()) {
                handleError(responseBody, it->second);
                responseBody.second = "text/html";
                _SET_RESPONSE_HEADERS(responseHeaders, responseBody);
                _response = HTTP::Response(HTTP::NOT_FOUND, HTTP::HTTP_1_1, responseHeaders, responseBody.first);
                return;
            } else {
                responseBody.first = "<html><body><h1>Error 404: Not Found</h1></body></html>";
                responseBody.second = "text/html";
            }
        }
    }

    // Set response headers
    _SET_RESPONSE_HEADERS(responseHeaders, responseBody);
    responseHeaders["Connection"] = HTTP::Header("Connection", "keep-alive");

    // Set the response
    _response = HTTP::Response(HTTP::OK, HTTP::HTTP_1_1, responseHeaders, responseBody.first);
}


void RequestHandler::handlePostRequest(std::pair<std::string, std::string> &responseBody, 
                                       std::map<std::string, HTTP::Header> &responseHeaders) {
    Logger::logMsg("INFO", CONSOLE_OUTPUT, "Handling POST request");

    std::string resourcePath = _server.getRoot() + _request.getResource();
    std::ofstream outFile(resourcePath.c_str(), std::ios::binary | std::ios::app);
    
    if (!outFile) {
        Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "Failed to open file for writing: %s", resourcePath.c_str());
        responseBody.first = "Error 500: Internal Server Error";
        responseBody.second = "html";
        _SET_RESPONSE_HEADERS(responseHeaders, responseBody);
        _response = HTTP::Response(HTTP::INTERNAL_SERVER_ERROR, HTTP::HTTP_1_1, responseHeaders, responseBody.first);
        return;
    }

    outFile << _request.getBody();
    outFile.close();
    
    responseBody.first = "POST request processed successfully";
    responseBody.second = "text/html";
    _SET_RESPONSE_HEADERS(responseHeaders, responseBody);
    responseHeaders["Connection"] = HTTP::Header("Connection", "keep-alive");
    _response = HTTP::Response(HTTP::OK, HTTP::HTTP_1_1, responseHeaders, responseBody.first);

    Logger::logMsg("INFO", CONSOLE_OUTPUT, "POST request handled successfully");
}


void RequestHandler::handleRemoveRequest(std::pair<std::string, std::string> &responseBody, 
                                         std::map<std::string, HTTP::Header> &responseHeaders) {
    Logger::logMsg("INFO", CONSOLE_OUTPUT, "Handling DELETE request");

    std::string resourcePath = _server.getRoot() + _request.getResource();
    
    if (remove(resourcePath.c_str()) != 0) {
        Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "Failed to delete file: %s", resourcePath.c_str());
        responseBody.first = "Error 404: Not Found";
        responseBody.second = "html";
        _SET_RESPONSE_HEADERS(responseHeaders, responseBody);
        _response = HTTP::Response(HTTP::NOT_FOUND, HTTP::HTTP_1_1, responseHeaders, responseBody.first);
        return;
    }

    responseBody.first = "DELETE request processed successfully";
    responseBody.second = "text/html";
    _SET_RESPONSE_HEADERS(responseHeaders, responseBody);
    responseHeaders["Connection"] = HTTP::Header("Connection", "keep-alive");
    _response = HTTP::Response(HTTP::OK, HTTP::HTTP_1_1, responseHeaders, responseBody.first);

    Logger::logMsg("INFO", CONSOLE_OUTPUT, "DELETE request handled successfully");
}

void RequestHandler::handleError(std::pair<std::string, std::string> &body, const std::string &errorPath) {
    body = readFile(_server.getRoot() + errorPath);
    if (body.first.empty()) {
        body.first = "Error: 404 Not Found";
        body.second = "html";
    }
}

void RequestHandler::handleAutoindex(std::pair<std::string, std::string> &responseBody, std::map<std::string, HTTP::Header> &responseHeaders, const std::string &directoryPath) {
    DIR *dir;
    struct dirent *ent;

    Logger::logMsg("INFO", CONSOLE_OUTPUT, "Directory path: %s", directoryPath.c_str());
    if ((dir = opendir(directoryPath.c_str())) != NULL) {
        std::stringstream ss;
        ss << "<html><body><h1>Index of " << directoryPath << "</h1><ul>";
        while ((ent = readdir(dir)) != NULL) {
            ss << "<li><a href=\"" << ent->d_name << "\">" << ent->d_name << "</a></li>";
        }
        ss << "</ul></body></html>";
        responseBody.first = ss.str();
        closedir(dir);
    } else {
        Logger::logMsg("ERROR", CONSOLE_OUTPUT, "Failed to open directory for autoindex: %s", directoryPath.c_str());
        responseBody.first = "Error: Could not open directory";
    }
    responseHeaders["Content-Type"] = HTTP::Header("Content-Type", "text/html");
    int length = responseBody.first.size();
    std::stringstream ss;
    ss << length;
    responseHeaders["Content-Length"] = HTTP::Header("Content-Length", ss.str());
    _response = HTTP::Response(HTTP::OK, HTTP::HTTP_1_1, responseHeaders, responseBody.first);
}

void RequestHandler::handleRequest() {
    std::map<std::string, HTTP::Header> responseHeaders;
    std::pair<std::string, std::string> responseBody;
    responseHeaders["Date"] = HTTP::Header("Date", utils::getCurrentDateTime());
    responseHeaders["Server"] = HTTP::Header("Server", "Webserv");

    // Get resource and directory
    std::string resource = _request.getResource();
    std::string directory = resource.substr(0, resource.find_last_of("/") + 1);
    
    // Log the request details
    Logger::logMsg("INFO", CONSOLE_OUTPUT, "Handling request for resource: %s", resource.c_str());

    // Find matching location
    std::vector<Location> locations = _server.getLocations();
    std::vector<Location>::const_iterator i = std::find_if(locations.begin(), locations.end(), ServerConfig::MatchLocation(directory));
    
    if (i == locations.end()) {
        responseBody = readFile(_server.getRoot() + resource);

        if (responseBody.first.empty()) {
            std::map<short, std::string>::const_iterator it = _errorPages.find(404);
            if (it != _errorPages.end() && !it->second.empty()) {
                Logger::logMsg("INFO", CONSOLE_OUTPUT, "Error page: %s", _errorPages[404].c_str());
                handleError(responseBody, _errorPages[404]);
            } else {
                responseBody.first = "<html><body><h1>Error 404: Not Found</h1></body></html>";
                responseBody.second = "html";
            }
            _SET_RESPONSE_HEADERS(responseHeaders, responseBody);
            _response = HTTP::Response(HTTP::NOT_FOUND, HTTP::HTTP_1_1, responseHeaders, responseBody.first);
        } else {
            _SET_RESPONSE_HEADERS(responseHeaders, responseBody);
            _response = HTTP::Response(HTTP::OK, HTTP::HTTP_1_1, responseHeaders, responseBody.first);
        }
        return;
    }

    // Check if the requested method is allowed
    std::vector<short> methods = i->getMethods();
    HTTP::Method method = _request.getMethod();
    if (method == HTTP::GET && methods[0])
        handleGetRequest(responseBody, responseHeaders, i, resource, directory);
    else if (method == HTTP::POST && methods[1])
        handlePostRequest(responseBody, responseHeaders);
    else if (method == HTTP::DELETE && methods[2])
        handleRemoveRequest(responseBody, responseHeaders);
    else {
        Logger::logMsg("ERROR", CONSOLE_OUTPUT, "Method not allowed");
        handleMethodNotAllowed(responseBody, responseHeaders);
    }
}

void RequestHandler::handleMethodNotAllowed(std::pair<std::string, std::string> &responseBody, std::map<std::string, HTTP::Header> &responseHeaders) {
    responseBody.first = "<html><body><h1>Error 405: Method Not Allowed</h1></body></html>";
    responseBody.second = "html";
    _SET_RESPONSE_HEADERS(responseHeaders, responseBody);
    _response = HTTP::Response(HTTP::METHOD_NOT_ALLOWED, HTTP::HTTP_1_1, responseHeaders, responseBody.first);
}
