#pragma once

#include "Http.hpp"
#include "ServerConfig.hpp"

class	RequestHandler {
	private:
		ServerConfig	_server;
		HTTP::Request   _request;
		HTTP::Response	_response;

		std::string readFile(const std::string &path);
		void				    handleGetRequest();
		void				    handleBadRequest();

		static std::map<std::string, std::string> fileCache;
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
		void					handleRequest();
		void					handleFindError(std::string &body, std::string &errorPath);

};
