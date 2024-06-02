#include "RequestHandler.hpp"

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
		// std::cout << "File found in cache" << std::endl;
		return fileCache[path];
	}
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << path << std::endl;
		return "";
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	fileCache[path] = buffer.str();
	return buffer.str();
}
std::string RequestHandler::normalizePath(const std::string &path) {
	std::string normalized = path;
	size_t pos = 0;
	while((pos = normalized.find("//", pos)) != std::string::npos) {
		normalized.replace(pos, 2, "/");
	}
	return normalized;
}
std::string RequestHandler::getFilePath(const std::string &resource, const std::string &root) {
	return root + "/" + resource;
}
void	RequestHandler::handleGetRequest() {
	std::map<std::string, HTTP::Header> responseHeaders;
	std::string responseBody;
	responseHeaders["Date"] = HTTP::Header("Date", utils::getCurrentDateTime());
	responseHeaders["Server"] = HTTP::Header("Server", "Webserv");
	
	std::string resource = _request.getResource();
	std::string root = _server.getRoot();
	std::string fulPath = getFilePath(resource, root);

	std::vector<Location> locations = _server.getLocations();
	bool handled = false;
	
	for (std::vector<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
		std::cout << RED << "returning: " << it->getReturn() << std::endl;
		if (it->getPath() == resource) {
			std::string mimeType = _server.getMimeType(it->getType());
			responseHeaders["Content-Type"] = HTTP::Header("Content-Type", mimeType);
			if (it->getAutoindex()) {
				responseBody = "Autoindex";
			} else {
				std::string filePath = normalizePath(it->getRootLocation() + "srcs/" + it->getPath());
				responseBody = readFile(filePath);
			}
			handled = true;
			break;
		}
	}
	if (!handled) {
		handleBadRequest();
		return;
	}
	int length = responseBody.size();
	std::stringstream ss;
	ss << length;
	responseHeaders["Content-Length"] = HTTP::Header("Content-Length", ss.str());
	_response = HTTP::Response(HTTP::OK, HTTP::HTTP_1_1, responseHeaders, responseBody);
}
void RequestHandler::handleBadRequest() {
    this->_response = HTTP::Response(HTTP::BAD_REQUEST, HTTP::HTTP_1_1, std::map<std::string, HTTP::Header>(), "Bad request");
}

// std::string RequestHandler::getHomeIndex() {
// 	return readFile("gameHub/srcs/indexes/home.html");
// }

// std::string RequestHandler::getHomeStyle() {
// 	return readFile("gameHub/srcs/styles/home.css");
// }

// // std::string RequestHandler::getHomeScript() {
// // 	return readFile("gameHub/srcs/scripts/home.js");
// // }

// std::string RequestHandler::getLogo() {
// 	return readFile("gameHub/imgs/games/logo.png");
// }

// std::string RequestHandler::getBouncingBalls() {
// 	return readFile("gameHub/imgs/games/bouncingBalls.png");
// }






void    RequestHandler::handleRequest() {
    try {
		// std::cout << YELLOW << "Request: " << this->getRequest().serialize() << RESET << std::endl;
        // !1. Parse the request:
        switch (this->_request.getMethod()) {
            case HTTP::GET:
                handleGetRequest();
                break;
            default:
                handleBadRequest();
    	}
    } catch (const std::exception &e) {
        std::cerr << RED << "Error: " << e.what() << RESET << std::endl;
		handleBadRequest();
    }
}