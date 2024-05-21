/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 11:52:26 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/21 17:40:42 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_FILE_PARSER_HPP
#define CONFIG_FILE_PARSER_HPP

#include "Webserv.hpp"

class ServerConfig;

class ConfigFileParser
{
	// !Constructor / Destructor
	public:
		ConfigFileParser();
		~ConfigFileParser();
	// !Private attributes
	private:
		std::vector<std::string>	_serversConfig;
		size_t						_numOfServers;
		std::vector<ServerConfig> _servers;
	public:
	// !Methods
		int parseConfigFile(std::string & configFilePath);
		std::vector<ServerConfig> getServers() const;
	//! Parsing auxiliar methods
		void removeComments(std::string & someString);
		void removeWhiteSpace(std::string &content);
	//! Split servers
		size_t findStartServer(size_t start, std::string &content);
		size_t findEndServer(size_t start, std::string &content);
		void findAndSplitServers(std::string &content);
	//! Server creation
		void createServer(std::string &config, ServerConfig &server);
		void handleListenDirective(ServerConfig &server, std::vector<std::string> &parametrs, size_t &i);
		void handleLocationDirective(ServerConfig &server, std::vector<std::string> &parametrs, size_t &i, bool &flag_loc);
		void handleHostDirective(ServerConfig &server, std::vector<std::string> &parametrs, size_t &i);
		void handleRootDirective(ServerConfig &server, std::vector<std::string> &parametrs, size_t &i);
		void handleErrorPageDirective(std::vector<std::string> &parametrs, size_t &i, std::vector<std::string> &error_codes);
		void handleClientMaxBodySizeDirective(ServerConfig &server, std::vector<std::string> &parametrs, size_t &i, bool &flag_max_size);
		void handleServerNameDirective(ServerConfig &server, std::vector<std::string> &parametrs, size_t &i);
		void handleIndexDirective(ServerConfig &server, std::vector<std::string> &parametrs, size_t &i);
		void handleAutoindexDirective(ServerConfig &server, std::vector<std::string> &parametrs, size_t &i, bool &flag_autoindex);
		void handleUnsupportedDirective(bool flag_loc);
		void finalizeServerConfig(ServerConfig &server, const std::vector<std::string> &error_codes);
		
	//! Check for duplicate servers
		void checkForDuplicateServers();
	
	//! Debugging
		int printServers();
	public: 
		class ParsingErrorException : public std::exception
		{
			private:
				std::string _message;
			public:
				ParsingErrorException(std::string message) throw()
				{
					_message = "Config File Parser Error: " + message;
				}
				virtual const char* what() const throw()
				{
					return (_message.c_str());
				}
				virtual ~ParsingErrorException() throw() {}
		};
		
};

#endif