#include "RequestHandler.hpp"

std::map<std::string, std::string> fileCache;

std::string readFile(const std::string &path) {
	if (fileCache.find(path) != fileCache.end()) {
		std::cout << "File found in cache" << std::endl;
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
	return root + resource;
}

std::string RequestHandler::getHomeIndex() {
	return readFile("gameHub/srcs/indexes/home.html");
}

std::string RequestHandler::getHomeStyle() {
	return readFile("gameHub/srcs/styles/home.css");
}

// std::string RequestHandler::getHomeScript() {
// 	return readFile("gameHub/srcs/scripts/home.js");
// }

std::string RequestHandler::getLogo() {
	return readFile("gameHub/imgs/games/logo.png");
}

std::string RequestHandler::getBouncingBalls() {
	return readFile("gameHub/imgs/games/bouncingBalls.png");
}

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

void	RequestHandler::handleGetRequest() {
	std::map<std::string, HTTP::Header> responseHeaders;
	std::string responseBody;
	responseHeaders["Date"] = HTTP::Header("Date", utils::getCurrentDateTime());
	responseHeaders["Server"] = HTTP::Header("Server", "Webserv");
	std::vector<Location> locations = _server.getLocations();
	std::string resource = _request.getResource();
	std::string root = _server.getRoot();
	for (std::vector<Location>::const_iterator it = locations.begin(); it != locations.end(); it++) {
		if (it->getPath() == resource) {
			std::string mimeType = _server.getMimeType(it->getType());
			std::string path = it->getPath();
			if (it->getAutoindex()) {
				responseHeaders["Content-Type"] = HTTP::Header("Content-Type", mimeType);
				responseBody = "Autoindex";
			} else {
				responseHeaders["Content-Type"] = HTTP::Header("Content-Type", mimeType);
				if (path == "/")
					responseBody = readFile(it->getIndexLocation());
				else
					responseBody = readFile(root + "srcs" + path);
			}
			int length = responseBody.size();
			std::stringstream ss;
			ss << length;
			responseHeaders["Content-Length"] = HTTP::Header("Content-Length", ss.str());
			_response = HTTP::Response(HTTP::OK, HTTP::HTTP_1_1, responseHeaders, responseBody);
			// if (mimeType != "image/png")
			// 	std::cout << _response.serialize() << std::endl;
		}
		else if (it == locations.end())
			handleBadRequest();
	}
}

void RequestHandler::handleBadRequest() {
    this->_response = HTTP::Response(HTTP::BAD_REQUEST, HTTP::HTTP_1_1, std::map<std::string, HTTP::Header>(), "Bad request");
}

void    RequestHandler::handleRequest() {
    try {
		// std::cout << YELLOW << "Request: " << this->getRequest().serialize() << RESET << std::endl;
        // !1. Parse the request:
        switch (this->_request.getMethod()) {
            case HTTP::GET:
                handleGetRequest();
                break;
            // case HTTP::POST:
            //     this->_response = handlePostRequest(req, server);
            //     break;
            // case HTTP::PUT:
            //     this->_response = handlePutRequest(req, server);
            //     break;
            // case HTTP::DELETE:
            //     this->_response = handleDeleteRequest(req, server);
            //     break;
            // case HTTP::HEAD:
            //     this->_response = handleHeadRequest(req, server);
            //     break;
            // case HTTP::CONNECT:
            //     this->_response = handleConnectRequest(req, server);
            //     break;
            // case HTTP::OPTIONS:
            //     this->_response = handleOptionsRequest(req, server);
            //     break;
            // case HTTP::TRACE:
            //     this->_response = handleTraceRequest(req, server);
            //     break;
            // case HTTP::PATCH:
            //     this->_response = handlePatchRequest(req, server);
            //     break;
            default:
                handleBadRequest();
    }
		// !2. Determine the server:
		// Since we're passing a ServerConfig object to the function, we already have the server
		// information. We can use this information as needed in the following steps.

		// !3.Check the request against the server's configuration:
		// We might want to check if the request is valid according to the server's configuration.
		// For example, we could check if the request's target resource matches one of the server's
		// locations, if the request's body size is within the server's max body size, etc.

		// !4. Generate the response:
		// If the request is for a CGI script, you would run the script here, capture its output, and use that
		// to generate the response. If the request is not for a CGI script, you would generate the response as
		// you would normally.

		// !5. Serialize the response:
		// Use the HTTP::Response::serialize() method to convert the HTTP::Response object into a 
		// string that can be sent over the network.

        // 2. Determine the response based on the parsed request:
    //     initRoutes();
    //     std::map<std::string, std::pair<HTTP::Method, HTTP::Response(*)(/*const HTTP::Request&*/)> >& routes = getRoutes();
    //     std::map<std::string, std::pair<HTTP::Method, HTTP::Response(*)(/*const HTTP::Request&*/)> >::const_iterator it = routes.find(_request.getResource());
    //     if (it != routes.end() && it->second.first == _request.getMethod()) {
    //         HTTP::Response res = it->second.second(/*req*/);
    //         _response = res;
    //     } else
    //         _response = HTTP::Response(HTTP::NOT_FOUND, HTTP::HTTP_1_1, std::map<std::string, HTTP::Header>(), "");
    // } catch (const std::exception &e) {
    //     std::cerr << RED << "Error: " << e.what() << RESET << std::endl;
    // }
    // _response =  HTTP::Response(HTTP::INTERNAL_SERVER_ERROR, HTTP::HTTP_1_1, std::map<std::string, HTTP::Header>(), "");
    } catch (const std::exception &e) {
        std::cerr << RED << "Error: " << e.what() << RESET << std::endl;
		handleBadRequest();
    }

}