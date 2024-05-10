/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 11:52:26 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/10 11:48:36 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGFILEPARSER_HPP
#define CONFIGFILEPARSER_HPP

#include "Webserv.hpp"

class ConfigFileParser
{
	// Private attributes
	private:
		std::vector<std::string>	_serversConfig;
		size_t						_numOfServers;
		// std::vector<ServerConfig> _servers;
	
	// Constructor / Destructor
	public:
		ConfigFileParser();
		~ConfigFileParser();
	
	// Methods
	public:
		int parseConfigFile(std::string string);
		// std::vector<ServerConfig> getServers() const;
		// void setServers(std::vector<ServerConfig> servers);
		// void addServer(ServerConfig server);
		// void removeServer(ServerConfig server);
		// void removeServer(size_t index);
		// void clearServers();
		// size_t getNumOfServers() const;
		// void setNumOfServers(size_t numOfServers);
		// std::string toString() const;
		// std::string toFile() const;
		// void fromFile(std::string file);
		// void fromString(std::string string);
		// void fromServerConfig(ServerConfig server);
		// void fromServerConfig(std::vector<ServerConfig> servers);
		
};

#endif