/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 11:52:26 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/10 15:10:05 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"

class ServerConfig;

class ConfigFileParser
{
	// Private attributes
	private:
		std::vector<std::string>	_serversConfig;
		size_t						_numOfServers;
		std::vector<ServerConfig> _servers;
	
	// Constructor / Destructor
	public:
		ConfigFileParser();
		~ConfigFileParser();
	
	// Methods
	public:
		int parseConfigFile(std::string & configFilePath);
		std::vector<ServerConfig> getServers() const;
		void checkServers();

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
