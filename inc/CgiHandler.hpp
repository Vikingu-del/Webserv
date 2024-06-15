#pragma once

#include "Http.hpp"
#include "Client.hpp"
#include "ServerConfig.hpp"
#include <sys/epoll.h>

class CgiHandler {
	private:
		std::map<std::string, std::string>		_env;
		HTTP::Request							_req;
		HTTP::Response							_res;
		Client									_client;
		pid_t									_pid;
		std::string								_cgiExt;
		std::string								_resource;
		int										_pipeIn[2];
		int										_pipeOut[2];
		int										_epollFd;
		ServerConfig							_server;
		int 									_readBytes;
        int 									_readTime;
        int 									_statusCode;
        bool									_isRead;
        bool									_isParsed;
		int										_length;
        int										_exitStatus;

        // Private Methods
        void                initEnv();
        void	            closePipes();
		void	            deleteChild(int childFd);
        std::vector<char *> createEnvArray();

	public:
		CgiHandler(const Client &client, const HTTP::Request &req, int epollFd, std::string cgiExt, const ServerConfig &server);
		~CgiHandler();
		CgiHandler &operator=(const CgiHandler &copy);


        // Public Methods
        void initCgi();
        void execCgi();

		// Getters
		std::map<std::string, std::string>	&getEnv();
		ServerConfig						&getConfig();
		HTTP::Request						&getRequest();
		HTTP::Response						&getResponse();
		Client								&getClient();
		pid_t								getPid();
		std::string							&getCgiExt();
		std::string							&getResource();
		std::vector<std::string>			&getEnvp();
		int									getPipeIn();
		int									getPipeOut();
		int									getEpollFd();
		int									getReadBytes();
		int									getReadTime();
		int									getStatusCode();
		bool								getIsRead();
		bool								getIsParsed();
		int									getLength();
		int									getExitStatus();

		// Setters
		void	setEnv(const std::map<std::string, std::string> &env);
		void	setConfig(const ServerConfig &config);
		void	setRequest(const HTTP::Request &req);
		void	setResponse(const HTTP::Response &res);
		void	setClient(const Client &client);
		void	setPid(pid_t pid);
		void	setCgiExt(const std::string &cgiExt);
		void	setResource(const std::string &resource);
		void	setPipeIn(int pipeIn);
		void	setPipeOut(int pipeOut);
		void	setEpollFd(int epollFd);
		void	setReadBytes(int readBytes);
		void	setReadTime(int readTime);
		void	setStatusCode(int statusCode);
		void	setIsRead(bool isRead);
		void	setIsParsed(bool isParsed);
		void	setLength(int length);
		void	setExitStatus(int exitStatus);
};
