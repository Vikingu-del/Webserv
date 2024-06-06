#pragma once

#include "Http.hpp"
#include "ServerConfig.hpp"
#include <dirent.h>

class	RequestHandler {
	private:
		ServerConfig					_server;
		HTTP::Request   				_request;
		HTTP::Response					_response;
		std::map<short, std::string>	_errorPages;

		static std::map<std::string, std::string>	fileCache;
		std::pair<std::string, std::string> readFile(const std::string &path);
		void	handleGetRequest(std::pair<std::string, std::string> &responseBody,
					std::map<std::string, 
					HTTP::Header> &responseHeaders,
					std::vector<Location>::const_iterator &it,
					std::string &resource,
					std::string &directoryPath);
		void	handlePostRequest(std::pair<std::string, std::string> &responseBody, std::map<std::string, HTTP::Header> &responseHeaders);
		void	handleRemoveRequest(std::pair<std::string, std::string> &responseBody, std::map<std::string, HTTP::Header> &responseHeaders);
		void	handleError(std::pair<std::string, std::string> &responseBody, const std::string &errorPath);
		void	handleAutoindex(std::pair<std::string, std::string> &responseBody, std::map<std::string, HTTP::Header> &responseHeaders, const std::string &directoryPath);
		void	handleMethodNotAllowed(std::pair<std::string, std::string> &responseBody,
													std::map<std::string, HTTP::Header> &responseHeaders);
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
		void    setErrorPages(const std::map<short, std::string> &errorPages);

		// Methods
		void		handleRequest();
};
