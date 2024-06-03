#pragma once

#include "Http.hpp"
#include "ServerConfig.hpp"

class	RequestHandler {
	private:
		ServerConfig	_server;
		HTTP::Request   _request;
		HTTP::Response	_response;

		static std::map<std::string, std::string> fileCache;
		std::string readFile(const std::string &path);
		void	handleGetRequest(std::string &responseBody,
					std::map<std::string, 
					HTTP::Header> &responseHeaders,
					std::vector<Location>::const_iterator &it,
					std::string &resource);
		void        handleError(std::string &responseBody, const std::string &errorPath);
		void setResponseHeaders(std::map<std::string, HTTP::Header> &responseHeaders, 
								const std::string &contentType, 
								const std::string &responseBody);
	public:
		RequestHandler();
		RequestHandler(const ServerConfig &server, const std::string &request);

		// Getters
		const ServerConfig&					getServer() const;
		const HTTP::Request&				getRequest() const;
		const HTTP::Response&				getResponse() const;

		// Setters
		void	setServer(const ServerConfig &server);
		void	setRequest(const HTTP::Request &request);
		void	setResponse(const HTTP::Response &response);

		// Methods
		void		handleRequest();
};
