#pragma once

#include "Http.hpp"
#include "ServerConfig.hpp"

class RequestHandler {
    private:
        ServerConfig	_server;
        std::string		_request;
		HTTP::Response	_response;
		RequestHandler();
	public:
		RequestHandler(const ServerConfig &server, const std::string &request);

		// Getters
		const ServerConfig & getServer() const;
		const std::string & getRequest() const;
		const HTTP::Response & getResponse() const;

		// Setters
		void setServer(const ServerConfig &server);
		void setRequest(const std::string &request);
		void setResponse(HTTP::Response &response);

		// Methods
		std::map<std::string, std::pair<HTTP::Method, HTTP::Response(*)(/*const Request&*/)> >& getRoutes();
		HTTP::Response	getHome(/*const HTTP::Request &req*/);
		void	initRoutes();
		void	handleRequest(const std::string &request, const ServerConfig &server);
};
