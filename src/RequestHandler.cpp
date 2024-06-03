#include "RequestHandler.hpp"
#include <algorithm>

std::map<std::string, std::string> RequestHandler::fileCache;

RequestHandler::RequestHandler() : _server(), _request(), _response() {}

RequestHandler::RequestHandler(const ServerConfig &server, const std::string &request) : _server(server), _request(HTTP::Request::deserialize(request)), _response() {}

const ServerConfig& RequestHandler::getServer() const {
    return this->_server;
}

const HTTP::Request& RequestHandler::getRequest() const {
    return this->_request;
}

const HTTP::Response& RequestHandler::getResponse() const {
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

std::string RequestHandler::readFile(const std::string &path) {
	if (fileCache.find(path) != fileCache.end()) {
		std::cout << "File found in cache" << std::endl;
		return fileCache[path];
	}
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << path << std::endl;
		std::string body = readFile("gameHub/error_pages/404.html");
		return body;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	fileCache[path] = buffer.str();
	return buffer.str();
}

void	RequestHandler::handleGetRequest(std::string &responseBody, std::map<std::string, HTTP::Header> &responseHeaders, std::vector<Location>::const_iterator &i, std::string &resource) {
	responseHeaders["Content-Type"] = HTTP::Header("Content-Type", _server.getMimeType(i->getType()));
	if (i->getAutoindex()) {
		responseBody = "Autoindex";
	} else {
		std::string filePath = resource == "/" ? i->getIndexLocation() : i->getRootLocation() + resource;
		std::cout << "File Path: " << filePath << std::endl;
		responseBody = readFile(filePath);
	}
	// if (i->getType() != "png")
	// 	std::cout << RED << "Response Body: " << responseBody << std::endl;
	int length = responseBody.size();
	std::stringstream ss;
	ss << length;
	responseHeaders["Content-Length"] = HTTP::Header("Content-Length", ss.str());
	_response = HTTP::Response(HTTP::OK, HTTP::HTTP_1_1, responseHeaders, responseBody);
}

void   RequestHandler::handleFindError(std::string &body, std::string &errorPath) {
	body = readFile(errorPath);
}

void    RequestHandler::handleRequest() {
	std::map<std::string, HTTP::Header> responseHeaders;
	std::string responseBody;
	responseHeaders["Date"] = HTTP::Header("Date", utils::getCurrentDateTime());
	responseHeaders["Server"] = HTTP::Header("Server", "Webserv");
	std::vector<Location> locations = _server.getLocations();
	std::string resource = _request.getResource();
	std::map<short, std::string> errors = _server.getErrorPages();
	std::vector<Location>::const_iterator i = std::find_if(locations.begin(), locations.end(), ServerConfig::MatchLocation(resource));
	if (i == locations.end()) {
		std::map<short, std::string>::const_iterator it = errors.find(404);
		if (it != errors.end() && !it->second.empty())
			handleFindError(responseBody, errors[404]);
		else
			responseBody = "Error: 404 Not Found";
		responseHeaders["Content-Type"] = HTTP::Header("Content-Type", "text/html");
		int length = responseBody.size();
		std::stringstream ss;
		ss << length;
		responseHeaders["Content-Length"] = HTTP::Header("Content-Length", ss.str());
		_response = HTTP::Response(HTTP::NOT_FOUND, HTTP::HTTP_1_1, responseHeaders, responseBody);
		return;
	}
	std::vector<short> methods = i->getMethods();
	if (_request.getMethod() == HTTP::GET && methods[0])
			handleGetRequest(responseBody, responseHeaders, i, resource);
	else {
		std::cout << "Bad request in action" << std::endl;
		std::map<short, std::string>::const_iterator it = errors.find(400);
		if (it != errors.end() && !it->second.empty()) {
            handleFindError(responseBody, errors[400]);
        } else {
            responseBody = "Error: Bad Request";
        }
		responseHeaders["Content-Type"] = HTTP::Header("Content-Type", "text/html");
		int length = responseBody.size();
		std::stringstream ss;
		ss << length;
		responseHeaders["Content-Length"] = HTTP::Header("Content-Length", ss.str());
		_response = HTTP::Response(HTTP::NOT_FOUND, HTTP::HTTP_1_1, responseHeaders, responseBody);
	}
}
