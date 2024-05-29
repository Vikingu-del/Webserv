#include "RequestHandler.hpp"

RequestHandler::RequestHandler() {}

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

std::map<std::string, std::pair<HTTP::Method, HTTP::Response(*)()> >& RequestHandler::getRoutes() {
    static std::map<std::string, std::pair<HTTP::Method, HTTP::Response(*)()> > routes;
    return routes;
}

std::string RequestHandler::getHomeIndex() {
	std::string body;
	std::ifstream file("gameHub/srcs/indexes/hello.html");
	if (file.is_open()) {
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}
	return ("");
}

std::string RequestHandler::getHomeStyle() {
    std::string body;
    std::ifstream file("gameHub/srcs/styles/home.css");
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
		// std::cout << "Buffer: " << buffer.str() << std::endl;
        return buffer.str();
    }
    return ("");
}

std::string RequestHandler::getLogo() {
	std::string body;
	std::ifstream file("gameHub/imgs/games/logo.png", std::ios::binary);
	if (file.is_open()) {
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}
	return ("");
}

std::string RequestHandler::getBouncingBalls() {
	std::string body;
	std::ifstream file("gameHub/imgs/games/bouncingBalls.png", std::ios::binary);
	if (file.is_open()) {
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}
	return ("");
}

// void RequestHandler::initRoutes() {
//     std::map<std::string, std::pair<HTTP::Method, HTTP::Response(*)(/*const HTTP::Request&*/)> >& routes = getRoutes();
//     routes.insert(std::make_pair("/", std::make_pair(HTTP::GET, getHome)));
//     routes.insert(std::make_pair("/home", std::make_pair(HTTP::GET, getHome)));
// }

void	RequestHandler::handleGetRequest() {
	std::map<std::string, HTTP::Header> responseHeaders;
	std::string responseBody;
	responseHeaders["Date"] = HTTP::Header("Date", utils::getCurrentDateTime());
	responseHeaders["Server"] = HTTP::Header("Server", "Webserv");
	if (_request.getResource() == "indexes/home.html" || _request.getResource() == "/" || _request.getResource() == "/home") {
		responseHeaders["Content-Type"] = HTTP::Header("Content-Type", "text/html");
		responseBody = RequestHandler::getHomeIndex();
	}
	else if (_request.getResource() == "/styles/home.css") {
		responseHeaders["Content-Type"] = HTTP::Header("Content-Type", "text/css");
		responseBody = RequestHandler::getHomeStyle();
		// std::cout << "Response body for css: " << responseBody << std::endl;
	}
	else if (_request.getResource() == "/imgs/games/bouncingBalls.png") {
		responseHeaders["Content-Type"] = HTTP::Header("Content-Type", "image/png");
		responseBody = RequestHandler::getBouncingBalls();
	}
	else if (_request.getResource() == "/imgs/games/logo.png") {
		responseHeaders["Content-Type"] = HTTP::Header("Content-Type", "image/png");
		responseBody = RequestHandler::getLogo();
	}
	
	// else if (firsLine == )
	int length = responseBody.size();
	std::stringstream ss;
	ss << length;
	responseHeaders["Content-Length"] = HTTP::Header("Content-Length", ss.str());
	_response = HTTP::Response(HTTP::OK, HTTP::HTTP_1_1, responseHeaders, responseBody);
	// std::cout << "Response: " << RED <<_response.serialize() << RESET << std::endl;
}

void RequestHandler::handleBadRequest() {
    this->_response = HTTP::Response(HTTP::BAD_REQUEST, HTTP::HTTP_1_1, std::map<std::string, HTTP::Header>(), "Bad request");
}

void    RequestHandler::handleRequest() {
    try {
		std::cout << YELLOW << "Request: " << this->getRequest().serialize() << RESET << std::endl;
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
    }

}