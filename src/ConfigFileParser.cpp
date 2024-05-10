/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 11:53:16 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/10 15:11:45 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ConfigFileParser.hpp"

// Constructors
ConfigFileParser::ConfigFileParser() {
	_numOfServers = 0;
}

// Destructor
ConfigFileParser::~ConfigFileParser() {
}

void ConfigFileParser::checkServers()
{
	std::vector<ServerConfig>::iterator it1;
	std::vector<ServerConfig>::iterator it2;

	for (it1 = this->_servers.begin(); it1 != this->_servers.end() - 1; it1++)
	{
		for (it2 = it1 + 1; it2 != this->_servers.end(); it2++)
		{
			if (it1->getPort() == it2->getPort() && it1->getHost() == it2->getHost() && it1->getServerName() == it2->getServerName())
				throw ParsingErrorException("Failed server validation");
		}
	}
}

// Methods
int ConfigFileParser::parseConfigFile(std::string & configFilePath) {
	
	std::string		content;
	ConfigFile		file(configFilePath);

	if (file.checkFileExistence(file.getPath()) != 1)
		throw ParsingErrorException("File is invalid");
	if (file.checkFile(file.getPath(), 4) == -1)
		throw ParsingErrorException("File is not accessible");
	content = file.getFileContent(configFilePath);
	if (content.empty())
		throw ParsingErrorException("File is empty");
	// removeComments(content);
	// removeWhiteSpace(content);
	// splitServers(content);
	if (this->_serversConfig.size() != this->_numOfServers)
		throw ParsingErrorException("Number of servers in configuration does not match expected count.");
	for (size_t i = 0; i < this->_numOfServers; i++)
	{
		ServerConfig server;
		// createServer(this->_serversConfig[i], server);
		this->_servers.push_back(server);
	}
	if (this->_numOfServers > 1)
		checkServers();
	return (0);
}

