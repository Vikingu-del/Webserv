/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 11:52:26 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/16 18:26:26 by eseferi          ###   ########.fr       */
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
		void checkServers();
	public:
	//! Removing methods
		void removeComments(std::string & someString);
		void removeWhiteSpace(std::string &content);
	public:
	//! Split servers
		size_t findStartServer(size_t start, std::string &content);
		size_t findEndServer(size_t start, std::string &content);
		void splitServers(std::string &content);
		void createServer(std::string &config, ServerConfig &server);
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