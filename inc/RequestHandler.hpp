#pragma once

#include "Http.hpp"
#include "ServerConfig.hpp"

class	RequestHandler {
	private:
		ServerConfig	_server;
		HTTP::Request   _request;
		HTTP::Response	_response;
	public:
		RequestHandler();
		RequestHandler(const ServerConfig &server, const std::string &request);

		// int             _binary;
		// Getters
		const ServerConfig&		getServer() const;
		const HTTP::Request&	getRequest() const;
		const HTTP::Response&	getResponse() const;

		// Setters
		void	setServer(const ServerConfig &server);
		void	setRequest(const HTTP::Request &request);
		void	setResponse(const HTTP::Response &response);

		// Methods
		static					std::map<std::string, std::pair<HTTP::Method, HTTP::Response(*)(/*const Request&*/)> >& getRoutes();
		static std::string		getHomeIndex();
		static std::string      getHomeStyle();
		static std::string      getLogo();
		static std::string		getBouncingBalls();
		static void				initRoutes();
		void					handleRequest();
		void				    handleGetRequest();
		void				    handlePostRequest();
		void				    handlePutRequest();
		void				    handleDeleteRequest();
		void				    handleOptionsRequest();
		void				    handleTraceRequest();
		void				    handleConnectRequest();
		void				    handlePatchRequest();
		void				    handleHeadRequest();
		void				    handleBadRequest();

};
