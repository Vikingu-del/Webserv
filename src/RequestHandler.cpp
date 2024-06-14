#include "RequestHandler.hpp"
#include <algorithm>

std::map<std::string, std::string> RequestHandler::fileCache;

RequestHandler::RequestHandler() : _server(), _request(), _response(), _errorPages(), _showAutoIndex(0) {}

RequestHandler::RequestHandler(const ServerConfig &server, const std::string &request) : _server(server), _request(HTTP::Request::deserialize(request)), _response(), _errorPages(server.getErrorPages()), _showAutoIndex(0) {}

const ServerConfig& RequestHandler::getServer() const {
    return this->_server;
}

const HTTP::Request& RequestHandler::getRequest() const {
    return this->_request;
}

const HTTP::Response& RequestHandler::getResponse() const {
    return this->_response;
}

void RequestHandler::setErrorPages(const std::map<short, std::string> &errorPages) {
	this->_errorPages = errorPages;
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

std::pair<std::string, std::string> RequestHandler::readFile(const std::string &path) {
	std::string extension = path.substr(path.find_last_of(".") + 1);
	if (fileCache.find(path) != fileCache.end()) {
		std::cout << "File found in cache" << std::endl;
		return std::make_pair(fileCache[path], extension);
	}
	if (_showAutoIndex == 1) _showAutoIndex = 0;
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

void RequestHandler::handleGetRequest(std::pair<std::string,std::string> &responseBody,
									std::map<std::string, HTTP::Header> &responseHeaders,
									std::vector<Location>::const_iterator &i,
									std::string &resource,
									std::string &directoryPath) {
	responseBody.second = _server.getMimeType(i->getType());
	std::cout << RED << "Directory: " << directoryPath << RESET << std::endl;
	std::cout << RED << "Resource: " << resource << RESET << std::endl;
	std::cout << RED << "Index location: " << i->getIndexLocation() << RESET << std::endl;
	std::cout << RED << "Root location: " << i->getRootLocation() << RESET << std::endl;
	std::string filePath = i->getRootLocation() + (directoryPath == resource ? i->getIndexLocation() : resource);
	std::cout << CYAN << "File path: " << filePath << RESET << std::endl;
	_showAutoIndex = i->getAutoindex();
	// ! Use a logging library instead of std::cout for production (FOR IVAN)
	responseBody = readFile(filePath);
	if (responseBody.first == "404 not found" || (responseBody.first == readFile(_errorPages[404]).first)) {
		if (i->getAutoindex() && _showAutoIndex == 0) {
			std::cout << RED << "Autoindex enabled: " << i->getRootLocation() + directoryPath << RESET << std::endl;
			handleAutoindex(responseBody, responseHeaders, i->getRootLocation() + directoryPath);
			return ;
		} else {
			std::map<short, std::string>::const_iterator it = _errorPages.find(404);
			if (it != _errorPages.end() && !it->second.empty()) {
				handleError(responseBody, it->second);
				responseBody.second = "text/html";
				_SET_RESPONSE_HEADERS(responseHeaders, responseBody);
				_response = HTTP::Response(HTTP::NOT_FOUND, HTTP::HTTP_1_1, responseHeaders, responseBody.first);
				return;
			} else {
				responseBody.first = "Error: 404 Not Found";
				responseBody.second = "text/html";
			}
		}
	}
	_SET_RESPONSE_HEADERS(responseHeaders, responseBody);
	responseHeaders["Connection"] = HTTP::Header("Connection", "keep-alive");
	_response = HTTP::Response(HTTP::OK, HTTP::HTTP_1_1, responseHeaders, responseBody.first);
}


void RequestHandler::handlePostRequest(std::pair<std::string, std::string> &responseBody, std::map<std::string, HTTP::Header> &responseHeaders) {
	// Implement logic for handling POST request
	responseBody.first = "POST request handling not implemented yet";
	_SET_RESPONSE_HEADERS(responseHeaders, responseBody);
	_response = HTTP::Response(HTTP::OK, HTTP::HTTP_1_1, responseHeaders, responseBody.first);
}

void RequestHandler::handleRemoveRequest(std::pair<std::string, std::string>&responseBody, std::map<std::string, HTTP::Header> &responseHeaders) {
	// Implement logic for handling DELETE request
	responseBody.first = "DELETE request handling not implemented yet";
	_SET_RESPONSE_HEADERS(responseHeaders, responseBody);
	_response = HTTP::Response(HTTP::OK, HTTP::HTTP_1_1, responseHeaders, responseBody.first);
}

void RequestHandler::handleError(std::pair<std::string, std::string> &body, const std::string &errorPath) {
    body = readFile("gameHub" + errorPath);
    if (body.first.empty()) {
        body.first = "Error: 404 Not Found";
		body.second = "html";
    }
}

void    RequestHandler::handleRequest() {
	std::map<std::string, HTTP::Header> responseHeaders;
	std::pair<std::string, std::string> responseBody;
	responseHeaders["Date"] = HTTP::Header("Date", utils::getCurrentDateTime());
	responseHeaders["Server"] = HTTP::Header("Server", "Webserv");
	std::vector<Location> locations = _server.getLocations();
	std::string resource = _request.getResource();
	std::string directory = resource.substr(0, resource.find_last_of("/") + 1);
	std::vector<Location>::const_iterator i = std::find_if(locations.begin(), locations.end(), ServerConfig::MatchLocation(directory));
	if (i == locations.end()) {  // here should be the logic in the case the location was not found
		std::cout << "Location not found" << std::endl;
		responseBody = readFile(_server.getRoot() + resource);
		if (responseBody.first.empty()) {
			std::map<short, std::string>::const_iterator it = _errorPages.find(404);
			if (it != _errorPages.end() && !it->second.empty()) {
				std::cout << RED << _errorPages[404] << RESET << std::endl;
				handleError(responseBody, _errorPages[404]);
			}
			else {
				responseBody.first = "Error: 404 Not Found";
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
	std::vector<short> methods = i->getMethods();  // here in case the location found and we should try to route the request
	switch (_request.getMethod()) {
        case HTTP::GET:
            if (methods[0]) handleGetRequest(responseBody, responseHeaders, i, resource , directory);
            else {
				std::cout << "Bad request in action" << std::endl;
        		std::map<short, std::string>::const_iterator it = _errorPages.find(405);
				if (it != _errorPages.end() && !it->second.empty()) {
					std::cout << RED << _errorPages[405] << RESET << std::endl;
					handleError(responseBody, _errorPages[405]);
				} else {
					responseBody.first = "Error 405: Bad Request";
					responseBody.second = "html";
				}
				_SET_RESPONSE_HEADERS(responseHeaders, responseBody);
				_response = HTTP::Response(HTTP::BAD_REQUEST, HTTP::HTTP_1_1, responseHeaders, responseBody.first);
			}
            break;
        case HTTP::POST:
            if (methods[1]) handlePostRequest(responseBody, responseHeaders);
            else handleMethodNotAllowed(responseBody, responseHeaders);
            break;
        case HTTP::DELETE:
            if (methods[2]) handleRemoveRequest(responseBody, responseHeaders);
            else handleMethodNotAllowed(responseBody, responseHeaders);
            break;
        default:
			handleMethodNotAllowed(responseBody, responseHeaders);
    }
}

void RequestHandler::handleAutoindex(std::pair<std::string, std::string> &responseBody, std::map<std::string, HTTP::Header> &responseHeaders, const std::string &directoryPath) {
    DIR *dir;
    struct dirent *ent;


	std::cout << RED << "Directory path: " << directoryPath << RESET << std::endl;
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
        // Could not open directory
        perror("Failed to open directory for autoindex");
        responseBody.first = "Error: Could not open directory";
    }
	std::cout << RED << responseBody.first << RESET << std::endl;
    responseHeaders["Content-Type"] = HTTP::Header("Content-Type", "text/html");
    int length = responseBody.first.size();
    std::stringstream ss;
    ss << length;
    responseHeaders["Content-Length"] = HTTP::Header("Content-Length", ss.str());
	_response = HTTP::Response(HTTP::OK, HTTP::HTTP_1_1, responseHeaders, responseBody.first);
}

void RequestHandler::handleBadRequest() {
	std::cout << "Trying to handle bad request" << std::endl;
    this->_response = HTTP::Response(HTTP::BAD_REQUEST, HTTP::HTTP_1_1, std::map<std::string, HTTP::Header>(), "Bad request");
}

void RequestHandler::handleFindError(std::string &body) {
	body = readFile("gameHub/error_pages/404.html");
}

void    RequestHandler::handleRequest() {
	// !1. Parse the request:
	switch (this->_request.getMethod()) {
		case HTTP::GET:
			handleGetRequest();
			break;
		default:
			std::cout << "Bad request in action" << std::endl;
			handleBadRequest();
	}
}