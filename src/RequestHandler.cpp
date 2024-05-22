#include "RequestHandler.hpp"

RequestHandler::RequestHandler() {}

RequestHandler::RequestHandler(const ServerConfig &server, const std::string &request) : _server(server), _request(request), _response() {}

const ServerConfig & RequestHandler::getServer() const {
    return this->_server;
}

const std::string & RequestHandler::getRequest() const {
    return this->_request;
}

const HTTP::Response & RequestHandler::getResponse() const {
    return this->_response;
}

void RequestHandler::setServer(const ServerConfig &server) {
    this->_server = server;
}

void RequestHandler::setRequest(const std::string &request) {
    this->_request = request;
}

void RequestHandler::setResponse(const HTTP::Response &response) {
    this->_response = response;
}

std::map<std::string, std::pair<HTTP::Method, HTTP::Response(*)(/*const HTTP::Request&*/)> >& getRoutes() {
    static std::map<std::string, std::pair<HTTP::Method, HTTP::Response(*)(/*const Request&*/)> > routes;
    return routes;
}

HTTP::Response RequestHandler::getHome(/*const HTTP::Request& req*/) {
	std::string body;
	std::ifstream file("gameHub/srcs/indexes/home.html");
	std::cout << "here is the file" << file.is_open() << std::endl;
	if (file.is_open()) {
		std::stringstream buffer;
		buffer << file.rdbuf();
		// std::cout << "\n here is the buffer" << buffer.str() << std::endl;
		body = buffer.str();
		return HTTP::Response(HTTP::OK, HTTP::HTTP_1_1, std::map<std::string, HTTP::Header>(), body);
	}
	return (HTTP::Response(HTTP::NOT_FOUND, HTTP::HTTP_1_1, std::map<std::string, HTTP::Header>(), ""));
}

void RequestHandler::initRoutes() {
    std::map<std::string, std::pair<HTTP::Method, HTTP::Response(*)(/*const HTTP::Request&*/)> >& routes = getRoutes();
    routes.insert(std::make_pair("/", std::make_pair(HTTP::GET, getHome())));
    routes.insert(std::make_pair("/home", std::make_pair(HTTP::GET, getHome())));
}

void    RequestHandler::handleRequest(const std::string &request, const ServerConfig &server) {
    try {
		ServerConfig newServer = server;
		std::cout << newServer.getPort() << std::endl;
        // !1. Parse the request:
        HTTP::Request req = HTTP::Request::deserialize(request);

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
        this->initRoutes();
        std::map<std::string, std::pair<HTTP::Method, HTTP::Response(*)(/*const HTTP::Request&*/)> >& routes = this->getRoutes();
        std::map<std::string, std::pair<HTTP::Method, HTTP::Response(*)(/*const HTTP::Request&*/)> >::const_iterator it = routes.find(req.getResource());
        if (it != routes.end() && it->second.first == req.getMethod()) {
            HTTP::Response res = it->second.second(/*req*/);
            _response = res;
        } else
            _response = HTTP::Response(HTTP::NOT_FOUND, HTTP::HTTP_1_1, std::map<std::string, HTTP::Header>(), "");
    } catch (const std::exception &e) {
        std::cerr << RED << "Error: " << e.what() << RESET << std::endl;
    }
    _response =  HTTP::Response(HTTP::INTERNAL_SERVER_ERROR, HTTP::HTTP_1_1, std::map<std::string, HTTP::Header>(), "");
}