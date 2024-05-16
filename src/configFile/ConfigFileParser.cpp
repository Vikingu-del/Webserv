/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 11:53:16 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/16 18:39:57 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFileParser.hpp"

// !Main method && Starting point of parsing
int ConfigFileParser::parseConfigFile(std::string & configFilePath) {
	// std::cout << CYAN BLD "ConfigFileParser parseConfigFile" RST << std::endl;
	std::string		content;
	ConfigFile		file(configFilePath);

	if (file.checkFileExistence(file.getPath()) != THIS_IS_FILE)
		throw ParsingErrorException("File is invalid");
	if (file.checkFilePermissons(file.getPath(), READ_PERMISSION) == -1)
		throw ParsingErrorException("File is not accessible");
	content = file.getFileContent(configFilePath);
	if (content.empty())
		throw ParsingErrorException("File is empty"); 
	removeComments(content);
	removeWhiteSpace(content);
	splitServers(content);
	if (this->_serversConfig.size() != this->_numOfServers)
		throw ParsingErrorException("Number of servers in configuration does not match expected count.");
	for (size_t i = 0; i < this->_numOfServers; i++)
	{
		ServerConfig server;
		createServer(this->_serversConfig[i], server);
		this->_servers.push_back(server);
	}
	if (this->_numOfServers > 1)
		checkServers();
	return (0);
}

//! This method just remove all the commets , fm: '#' to '\n'
void ConfigFileParser::removeComments(std::string &someString)
{
	size_t hashPos = someString.find('#');
	
	while (hashPos != std::string::npos)
	{
		size_t newLineCharPos = someString.find('\n', hashPos);
		
		if (newLineCharPos == std::string::npos) {
			someString.erase(hashPos, newLineCharPos - hashPos);
		}
		else {
			someString.erase(hashPos);
			break;
		}
		hashPos = someString.find('#', newLineCharPos);
	}
}

//! This method deletes all whitespaces in 
void ConfigFileParser::removeWhiteSpace(std::string &content)
{
    // Remove leading whitespace
    size_t firstNonSpace = content.find_first_not_of(" \t\n\r");
    if (firstNonSpace != std::string::npos)
    {
        content = content.substr(firstNonSpace);
    }
    else
    {
        // If the string contains only whitespace, clear it
        content.clear();
        return;
    }

    // Remove trailing whitespace
    size_t lastNonSpace = content.find_last_not_of(" \t\n\r");
    if (lastNonSpace != std::string::npos)
    {
        content = content.substr(0, lastNonSpace + 1);
    }
}


//! Spliting servers by server{} and push to vector
void ConfigFileParser::splitServers(std::string &content)
{
	size_t start = 0;
	size_t end = 1;

	if (content.find("server", 0) == std::string::npos)
		std::cerr << "Server did not find" << std::endl;
	while (start != end && start < content.length())
	{
		start = findStartServer(start, content);
		end = findEndServer(start, content);
		if (start == end)
			ParsingErrorException("problem with scope");
		this->_serversConfig.push_back(content.substr(start, end - start + 1));
		this->_numOfServers++;
		start = end + 1;
	}
	std::cout << GREEN << BLD "Number of servers: " RST << this->_numOfServers << std::endl;
}


/* spliting line by separator */
std::vector<std::string> splitParametrs(std::string line, std::string sep)
{
	// std::cout << CYAN BLD "splitParametrs in ConfigFileParser line:154 " RST << std::endl;
	// std::cout << CYAN BLD "splitParametrs in ConfigFileParser separator is: " << sep << "#" RST << std::endl;
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

//! Finding a server start and return the index of { start of server
size_t ConfigFileParser::findStartServer(size_t start, std::string &content)
{
	// std::cout << CYAN BLD "ConfigFileParser findStartServer" RST << std::endl;
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
	// std::cout << CYAN BLD "ConfigFileParser findEndServer" RST << std::endl;
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


/* creating Server from string and fill the value */
void ConfigFileParser::createServer(std::string &config, ServerConfig &server)
{
	// std::cout << CYAN BLD "ConfigFileParser createServer" RST << std::endl;
	std::vector<std::string>	parametrs;
	std::vector<std::string>	error_codes;
	int		flag_loc = 1;
	bool	flag_autoindex = false;
	bool	flag_max_size = false;

	parametrs = splitParametrs(config += ' ', std::string(" \n\t"));
	if (parametrs.size() < 3)
		std::cerr << "Failed server validation" << std::endl;
	for (size_t i = 0; i < parametrs.size(); i++)
	{
		if (parametrs[i] == "listen" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (server.getPort())
				std::cerr << "Port is duplicated" << std::endl;
			server.setPort(parametrs[++i]);
		}
		else if (parametrs[i] == "location" && (i + 1) < parametrs.size())
		{
			std::string	path;
			i++;
			if (parametrs[i] == "{" || parametrs[i] == "}")
				std::cerr << "Wrong character in server scope{}" << std::endl;
			path = parametrs[i];
			std::vector<std::string> codes;
			if (parametrs[++i] != "{")
				std::cerr << "Wrong character in server scope{}" << std::endl;
			i++;
			while (i < parametrs.size() && parametrs[i] != "}")
				codes.push_back(parametrs[i++]);
			server.setLocation(path, codes);
			if (i < parametrs.size() && parametrs[i] != "}")
				std::cerr << "Wrong character in server scope{}" << std::endl;
			flag_loc = 0;
		}
		else if (parametrs[i] == "host" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (server.getHost())
				std::cerr << "Host is duplicated" << std::endl;
			server.setHost(parametrs[++i]);
		}
		else if (parametrs[i] == "root" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (!server.getRoot().empty())
				std::cerr << "Root is duplicated" << std::endl;
			server.setRoot(parametrs[++i]);
		}
		else if (parametrs[i] == "error_page" && (i + 1) < parametrs.size() && flag_loc)
		{
			while (++i < parametrs.size())
			{
				error_codes.push_back(parametrs[i]);
				if (parametrs[i].find(';') != std::string::npos)
					break ;
				if (i + 1 >= parametrs.size())
					std::cerr << "Wrong character out of server scope{}" << std::endl;
			}
		}
		else if (parametrs[i] == "client_max_body_size" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (flag_max_size)
				std::cerr << "Client_max_body_size is duplicated" << std::endl;
			server.setClientMaxBodySize(parametrs[++i]);
			flag_max_size = true;
		}
		else if (parametrs[i] == "server_name" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (!server.getServerName().empty())
				std::cerr << "Server_name is duplicated" << std::endl;
			server.setServerName(parametrs[++i]);
		}
		else if (parametrs[i] == "index" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (!server.getIndex().empty())
				std::cerr << "Index is duplicated" << std::endl;
			server.setIndex(parametrs[++i]);
		}
		else if (parametrs[i] == "autoindex" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (flag_autoindex)
				std::cerr << "Autoindex of server is duplicated" << std::endl;
			server.setAutoindex(parametrs[++i]);
			flag_autoindex = true;
		}
		else if (parametrs[i] != "}" && parametrs[i] != "{")
		{
			if (!flag_loc)
				std::cerr << "Parametrs after location" << std::endl;
			else
				std::cerr << "Unsupported directive" << std::endl;
		}
	}
	if (server.getRoot().empty())
		server.setRoot("/;");
	if (server.getHost() == 0)
		server.setHost("localhost;");
	if (server.getIndex().empty())
		server.setIndex("index.html;");
	if (ConfigFile::checkFile(server.getRoot(), server.getIndex()))
		std::cerr << "Index from config file not found or unreadable" << std::endl;
	if (server.checkLocaitons())
		std::cerr << "Location is duplicated" << std::endl;
	if (!server.getPort())
		std::cerr << "Port not found" << std::endl;
	server.setErrorPages(error_codes);
	if (!server.isValidErrorPages())
		std::cerr << "Incorrect path for error page or number of error" << std::endl;
	// std::cout << YELLOW BLD "Server created" RST << std::endl;
	// std::cout << GREEN BLD "Server created" RST << std::endl;
	// std::cout << YELLOW BLD "Server created" RST << std::endl;
}



int ConfigFileParser::printServers()
{
	// std::cout << CYAN BLD "ConfigFileParser printServers" RST << std::endl;
	std::cout << "------------- Config -------------" << std::endl;
	for (size_t i = 0; i < _servers.size(); i++)
	{
		std::cout << GREEN << BLD << "Server #" << RST << i + 1 << std::endl;
		std::cout << "Server name: " << _servers[i].getServerName() << std::endl;
		std::cout << "Host: " << _servers[i].getHost() << std::endl;
		std::cout << "Root: " << _servers[i].getRoot() << std::endl;
		std::cout << "Index: " << _servers[i].getIndex() << std::endl;
		std::cout << "Port: " << _servers[i].getPort() << std::endl;
		std::cout << "Max BSize: " << _servers[i].getClientMaxBody() << std::endl;
		std::cout << "Error pages: " << _servers[i].getErrorPages().size() << std::endl;
		std::map<short, std::string>::const_iterator it = _servers[i].getErrorPages().begin();
		while (it != _servers[i].getErrorPages().end())
		{
			std::cout << (*it).first << " - " << it->second << std::endl;
			++it;
		}
		std::cout << "Locations: " << _servers[i].getLocations().size() << std::endl;
		std::vector<Location>::const_iterator itl = _servers[i].getLocations().begin();
		while (itl != _servers[i].getLocations().end())
		{
			std::cout << "name location: " << itl->getPath() << std::endl;
			std::cout << "methods: " << itl->getPrintMethods() << std::endl;
			std::cout << "index: " << itl->getIndexLocation() << std::endl;
			if (itl->getCgiPath().empty())
			{
				std::cout << "root: " << itl->getRootLocation() << std::endl;
				if (!itl->getReturn().empty())
					std::cout << "return: " << itl->getReturn() << std::endl;
				if (!itl->getAlias().empty())
					std::cout << "alias: " << itl->getAlias() << std::endl;
			}
			else
			{
				std::cout << "cgi root: " << itl->getRootLocation() << std::endl;
				std::cout << "sgi_path: " << itl->getCgiPath().size() << std::endl;
				std::cout << "sgi_ext: " << itl->getCgiExtension().size() << std::endl;
			}
			++itl;
		}
		itl = _servers[i].getLocations().begin();
		std::cout << "-----------------------------" << std::endl;
	}
	return (0);
}

// !Constructors
ConfigFileParser::ConfigFileParser() {
	// std::cout << YELLOW "ConfigFileParser constructor" RST << std::endl;
	_numOfServers = 0;
}

// !Destructor
ConfigFileParser::~ConfigFileParser() {
	// std::cout << RED "ConfigFileParser destructor" RST << std::endl;
}

void ConfigFileParser::checkServers()
{
	// std::cout << CYAN BLD "ConfigFileParser checkServers" RST << std::endl;
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
