/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 11:53:16 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/27 14:09:39 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFileParser.hpp"

// !Main method && Starting point of parsing
int ConfigFileParser::parseConfigFile(std::string & configFilePath) {
	// std::cout << PURPLE << "parseConfigFile" << RESET << std::endl;
	std::string		content;
	ConfigFile		file(configFilePath);

	if (file.checkFileExistence(file.getPath()) != THIS_IS_FILE)
		throw ParsingErrorException("File is invalid");
	if (file.checkFilePermissons(file.getPath(), R_OK) == -1)
		throw ParsingErrorException("File is not accessible");
	content = file.getFileContent(configFilePath);
	if (content.empty())
		throw ParsingErrorException("File is empty"); 
	removeComments(content);
	removeWhiteSpace(content);
	// Also here we need to catch the exceptions from findAndSplitServers and throw them (ERIK)
	try { // I put this try catch block here because we need to catch the exceptions from the functions used here and throw them (ERIK)
		findAndSplitServers(content);
		for (size_t i = 0; i < this->_numOfServers; i++)
		{
			ServerConfig server;
			createServer(this->_serversConfig[i], server);
			this->_servers.push_back(server);
		}
		if (this->_numOfServers > 1)
			checkForDuplicateServers();
	} catch (ParsingErrorException & e) {
		throw ParsingErrorException(e.what());
	}
	std::cout << GREEN BLD "Success" RST << std::endl;
	return (0);
}

//! This method just remove all the commets , fm: '#' to '\n'
void ConfigFileParser::removeComments(std::string &str)
{
	// Do you think is better to use for loop instead of while loop? (ERIK)
	for (size_t hashPos = str.find('#'); hashPos != std::string::npos; hashPos = str.find('#', hashPos)) {
        size_t newLineCharPos = str.find('\n', hashPos);
        if (newLineCharPos != std::string::npos)
            str.erase(hashPos, newLineCharPos - hashPos);
        else {
            str.erase(hashPos);
            break;
        }
    }
}

//! This method deletes all whitespaces in 
void ConfigFileParser::removeWhiteSpace(std::string &content) {
	size_t firstNonSpace = content.find_first_not_of(" \t\n\r");
	if (firstNonSpace != std::string::npos)
		content = content.substr(firstNonSpace);
	else {
		content.clear();
		return;
	}
	size_t lastNonSpace = content.find_last_not_of(" \t\n\r");
	if (lastNonSpace != std::string::npos)
		content = content.substr(0, lastNonSpace + 1);
}

//! Spliting servers by server{} and push to vector
void ConfigFileParser::findAndSplitServers(std::string &content)
{
	size_t start = 0;
	size_t end = 1;

	if (content.find("server", 0) == std::string::npos)
		throw ParsingErrorException("'server' keyword not found");
	// Changed this part since we have to catch the exceptions from findStartServer and throw them (ERIK)
	while (start != end && start < content.length())
	{
		try {
			start = findStartServer(start, content);
			end = findEndServer(start, content);
			if (start == end)
				throw ParsingErrorException("problem with scope");
			this->_serversConfig.push_back(content.substr(start, end - start + 1));
			this->_numOfServers++;
			start = end + 1;
		}
		catch (ParsingErrorException & e) {
			throw ParsingErrorException(e.what());
		}
	}
}



//! Finding a server start and return the index of { start of server
size_t ConfigFileParser::findStartServer(size_t start, std::string &content)
{
	size_t serverPos = content.find("server", start);
	if (serverPos == std::string::npos)
		throw ParsingErrorException("Server keyword not found");
	size_t openBracePos = content.find_first_not_of(" \t\n", serverPos + 6);
	if (openBracePos == std::string::npos || content[openBracePos] != '{')
		throw ParsingErrorException("Expected '{' after server keyword");
	return openBracePos;
}

//! Finding a server end and return the index of } end of server
size_t ConfigFileParser::findEndServer (size_t start, std::string &content)
{
	size_t	i;
	size_t	scope;
	
	scope = 0;
	for (i = start + 1; content[i]; i++)
	{
		if (content[i] == '{')
			scope++;
		if (content[i] == '}')
		{
			if (!scope)
				return (i);
			scope--;
		}
	}
	return (start);
}

//! Spliting parametrs by separator
std::vector<std::string> splitParametrs(std::string line, std::string sep)
{
	std::vector<std::string>	str;
	std::string::size_type		start, end;

	start = end = 0;
	while (1)
	{
		end = line.find_first_of(sep, start);
		if (end == std::string::npos)
			break;
		std::string tmp = line.substr(start, end - start);
		str.push_back(tmp);
		start = line.find_first_not_of(sep, end);
		if (start == std::string::npos)
			break;
	}
	return (str);
}

//! Creating server by parametrs
void ConfigFileParser::createServer(std::string &config, ServerConfig &server) {
	// std::cout << RED << "createServer" << RESET << std::endl;
	std::vector<std::string> parametrs = splitParametrs(config += ' ', std::string(" \n\t"));
	if (parametrs.size() < 3)
		throw ParsingErrorException("Failed server validation");
	bool flag_loc = true;
	bool flag_autoindex = false;
	bool flag_max_size = false;
	std::vector<std::string> error_codes;
	try {
		for (size_t i = 0; i < parametrs.size(); i++) {
			if (parametrs[i] == "listen" && (i + 1) < parametrs.size() && flag_loc) {
				handleListenDirective(server, parametrs, i);
			} else if (parametrs[i] == "location" && (i + 1) < parametrs.size()) {
				handleLocationDirective(server, parametrs, i, flag_loc);
			} else if (parametrs[i] == "host" && (i + 1) < parametrs.size() && flag_loc) {
				handleHostDirective(server, parametrs, i);
			} else if (parametrs[i] == "root" && (i + 1) < parametrs.size() && flag_loc) {
				handleRootDirective(server, parametrs, i);
			} else if (parametrs[i] == "error_page" && (i + 1) < parametrs.size() && flag_loc) {
				handleErrorPageDirective(parametrs, i, error_codes);
			} else if (parametrs[i] == "client_max_body_size" && (i + 1) < parametrs.size() && flag_loc) {
				handleClientMaxBodySizeDirective(server, parametrs, i, flag_max_size);
			} else if (parametrs[i] == "server_name" && (i + 1) < parametrs.size() && flag_loc) {
				handleServerNameDirective(server, parametrs, i);
			} else if (parametrs[i] == "index" && (i + 1) < parametrs.size() && flag_loc) {
				handleIndexDirective(server, parametrs, i);
			} else if (parametrs[i] == "autoindex" && (i + 1) < parametrs.size() && flag_loc) {
				handleAutoindexDirective(server, parametrs, i, flag_autoindex);
			} else if (parametrs[i] != "}" && parametrs[i] != "{") {
				handleUnsupportedDirective(flag_loc);
			}
		}
		finalizeServerConfig(server, error_codes);
	} catch (ParsingErrorException & e) {
		throw ParsingErrorException(e.what());
	}
}

void ConfigFileParser::handleListenDirective(ServerConfig &server, std::vector<std::string> &parametrs, size_t &i) {
	if (server.getPort())
		throw ParsingErrorException("Port is duplicated");
	server.setPort(parametrs[++i]);
}

void ConfigFileParser::handleLocationDirective(ServerConfig &server, std::vector<std::string> &parametrs, size_t &i, bool &flag_loc) {
	std::string path;
	i++;
	if (parametrs[i] == "{" || parametrs[i] == "}")
		throw ParsingErrorException("Wrong character in server scope{}");
	path = parametrs[i];
	std::vector<std::string> codes;
	if (parametrs[++i] != "{")
		throw ParsingErrorException("Wrong character in server scope{}");
	i++;
	while (i < parametrs.size() && parametrs[i] != "}")
		codes.push_back(parametrs[i++]);
	server.setLocation(path, codes);
	if (i < parametrs.size() && parametrs[i] != "}")
		throw ParsingErrorException("Wrong character in server scope{}");
	flag_loc = false;
}

void ConfigFileParser::handleHostDirective(ServerConfig &server, std::vector<std::string> &parametrs, size_t &i) {
	// std::cout << "handleHostDirective" << std::endl;
	if (server.getHost())
		throw ParsingErrorException("Host is duplicated");
	server.setHost(parametrs[++i]);
}

void ConfigFileParser::handleRootDirective(ServerConfig &server, std::vector<std::string> &parametrs, size_t &i) {
	// std::cout << "handleRootDirective" << std::endl;
	if (!server.getRoot().empty())
		throw ParsingErrorException("Root is duplicated");
	server.setRoot(parametrs[++i]);
}

void ConfigFileParser::handleErrorPageDirective(std::vector<std::string> &parametrs, size_t &i, std::vector<std::string> &error_codes) {
	// std::cout << "handleErrorPageDirective" << std::endl;
	while (++i < parametrs.size()) {
		error_codes.push_back(parametrs[i]);
		if (parametrs[i].find(';') != std::string::npos)
			break;
		if (i + 1 >= parametrs.size())
			throw ParsingErrorException("Wrong character out of server scope{}");
	}
}

void ConfigFileParser::handleClientMaxBodySizeDirective(ServerConfig &server, std::vector<std::string> &parametrs, size_t &i, bool &flag_max_size) {
	// std::cout << "handleClientMaxBodySizeDirective" << std::endl;
	if (flag_max_size)
		throw ParsingErrorException("Client_max_body_size is duplicated");
	server.setClientMaxBodySize(parametrs[++i]);
	flag_max_size = true;
}

void ConfigFileParser::handleServerNameDirective(ServerConfig &server, std::vector<std::string> &parametrs, size_t &i) {
	// std::cout << "handleServerNameDirective" << std::endl;
	if (!server.getServerName().empty())
		throw ParsingErrorException("Server_name is duplicated");
	server.setServerName(parametrs[++i]);
}

void ConfigFileParser::handleIndexDirective(ServerConfig &server, std::vector<std::string> &parametrs, size_t &i) {
	// std::cout << "handleIndexDirective" << std::endl;
	if (!server.getIndex().empty())
		throw ParsingErrorException("Index is duplicated");
	server.setIndex(parametrs[++i]);
}

void ConfigFileParser::handleAutoindexDirective(ServerConfig &server, std::vector<std::string> &parametrs, size_t &i, bool &flag_autoindex) {
	// std::cout << "handleAutoindexDirective" << std::endl;
	if (flag_autoindex)
		throw ParsingErrorException("Autoindex of server is duplicated");
	server.setAutoindex(parametrs[++i]);
	flag_autoindex = true;
}

void ConfigFileParser::handleUnsupportedDirective(bool flag_loc) {
	// std::cout << "handleUnsupportedDirective" << std::endl;
	if (!flag_loc)
		throw ParsingErrorException("Parametrs after location");
	else
		throw ParsingErrorException("Unsupported directive");
}

void ConfigFileParser::finalizeServerConfig(ServerConfig &server, const std::vector<std::string> &error_codes) {
	// std::cout << "finalizeServerConfig" << std::endl;
	if (server.getRoot().empty())
		server.setRoot("/;");
	if (server.getHost() == 0)
		server.setHost("localhost;");
	if (server.getIndex().empty())
		server.setIndex("home.html;");
	if (ConfigFile::checkFile(server.getRoot(), server.getIndex()))
		throw ParsingErrorException("Index from config file not found or unreadable");
	if (server.checkLocaitons())
		throw ParsingErrorException("Location is duplicated");
	if (!server.getPort())
		throw ParsingErrorException("Port not found");
	server.setErrorPages(error_codes);
	if (!server.isValidErrorPages())
		throw ParsingErrorException("Incorrect path for error page or number of error");
}


//! Checking servers for duplicates
void ConfigFileParser::checkForDuplicateServers()
{
	// std::cout << "checkForDuplicateServers" << std::endl;
	std::vector<ServerConfig>::iterator it1;
	std::vector<ServerConfig>::iterator it2;

	for (it1 = this->_servers.begin(); it1 != this->_servers.end() - 1; it1++)
	{
		for (it2 = it1 + 1; it2 != this->_servers.end(); it2++)
		{
			if (it1->getPort() == it2->getPort() && it1->getHost() == it2->getHost() && it1->getServerName() == it2->getServerName())
				throw ParsingErrorException("Failed server validation, duplicate servers found");
		}
	}
}

// !Constructors
ConfigFileParser::ConfigFileParser() {
	// std::cout << "ConfigFileParser Construtor" << std::endl;
	_numOfServers = 0;
}

// !Destructor
ConfigFileParser::~ConfigFileParser() {
	// std::cout << "ConfigFileParser Destructor" << std::endl;
}

std::vector<ServerConfig>	ConfigFileParser::getServers() const
{
	return (this->_servers);
}

// !Debugging Method
int ConfigFileParser::printServers()
{

	std::cout << "------------- Config -------------" << std::endl;
	for (size_t i = 0; i < _servers.size(); i++)
	{
		std::cout << GREEN << BLD << "Server # " << RST BLD << i + 1 << RST << std::endl;
		std::cout << GREEN << "Server name: " << RST << _servers[i].getServerName() << std::endl;
		std::cout << GREEN << "Host: " << RST << _servers[i].getHost() << std::endl;
		std::cout << GREEN << "Root: " << RST << _servers[i].getRoot() << std::endl;
		std::cout << GREEN << "Index: " << RST << _servers[i].getIndex() << std::endl;
		std::cout << GREEN << "Port: " << RST << _servers[i].getPort() << std::endl;
		std::cout << GREEN << "Max BSize: " << RST << _servers[i].getClientMaxBody() << std::endl;
		std::cout << GREEN << "Error pages: " << RST << _servers[i].getErrorPages().size() << std::endl;
		std::map<short, std::string>::const_iterator it = _servers[i].getErrorPages().begin();
		while (it != _servers[i].getErrorPages().end())
		{
			std::cout << YELLOW << (*it).first << RST << " - " << YELLOW << it->second << RST << std::endl;
			++it;
		}
		std::cout << GREEN << "Locations: " << RST << _servers[i].getLocations().size() << std::endl;
		std::vector<Location>::const_iterator itl = _servers[i].getLocations().begin();
		while (itl != _servers[i].getLocations().end())
		{
			std::cout << PURPLE << "name location: " << RST << itl->getPath() << std::endl;
			std::cout << PURPLE << "methods: " << RST << itl->getPrintMethods() << std::endl;
			std::cout << PURPLE << "index: " << RST << itl->getIndexLocation() << std::endl;
			if (itl->getCgiPath().empty())
			{
				std::cout << PURPLE << "root: " << RST << itl->getRootLocation() << std::endl;
				if (!itl->getReturn().empty())
					std::cout << PURPLE << "return: " << RST << itl->getReturn() << std::endl;
				if (!itl->getAlias().empty())
					std::cout << PURPLE << "alias: " << RST << itl->getAlias() << std::endl;
			}
			else
			{
				std::cout << PURPLE << "cgi root: "  << RST << itl->getRootLocation() << std::endl;
				std::cout << PURPLE << "sgi_path: "  << RST << itl->getCgiPath().size() << std::endl;
				std::cout << PURPLE << "sgi_ext: "  << RST << itl->getCgiExtension().size() << std::endl;
			}
			++itl;
		}
		itl = _servers[i].getLocations().begin();
		std::cout << BLD << "-----------------------------" << RST << std::endl;
	}
	return (0);
}