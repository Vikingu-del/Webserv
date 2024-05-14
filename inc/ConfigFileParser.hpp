/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 11:52:26 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/14 19:20:04 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"

class ServerConfig;

class ConfigFileParser
{
	// !Private attributes
	private:
		std::vector<std::string>	_serversConfig;
		size_t						_numOfServers;
		std::vector<ServerConfig> _servers;
	
	// !Constructor / Destructor
	public:
		ConfigFileParser();
		~ConfigFileParser();
	
	// !Methods
		int parseConfigFile(std::string & configFilePath);
		std::vector<ServerConfig> getServers() const;
		void checkServers();
	
	//! Removing methods
		void removeComments(std::string & someString);
		void removeWhiteSpace(std::string &content);

	//! Split servers
		size_t findStartServer(size_t start, std::string &content);
		size_t findEndServer(size_t start, std::string &content);
		void splitServers(std::string &content);
		void createServer(std::string &config, ServerConfig &server);
		
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
