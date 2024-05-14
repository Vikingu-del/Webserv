/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 11:53:16 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/14 19:16:41 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ConfigFileParser.hpp"

// !Constructors
ConfigFileParser::ConfigFileParser() {
	_numOfServers = 0;
}

// !Destructor
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

// !Main method 
int ConfigFileParser::parseConfigFile(std::string & configFilePath) {
	
	std::string		content;
	ConfigFile		file(configFilePath);

	if (file.checkFileExistence(file.getPath()) != 1)
		throw ParsingErrorException("File is invalid");
	if (file.checkFilePermissons(file.getPath(), 4) == -1)
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
		// TODO: createServer(this->_serversConfig[i], server);
		this->_servers.push_back(server);
	}
	if (this->_numOfServers > 1)
		checkServers();
	return (0);
}

//! This method just remove all the commets , fm: '#' to '\n'
void ConfigFileParser::removeComments(std::string &someString) {
	size_t hashPos;

	hashPos = someString.find('#');
	
	while (hashPos != std::string::npos)
	{
		size_t newLineCharPos;
		newLineCharPos = someString.find('\n');
		someString.erase(hashPos, newLineCharPos - hashPos); //? params are (starting position, how many chars to delete)
		hashPos = someString.find('#');
	}
}

//! This method deletes all whitespaces in 
void ConfigFileParser::removeWhiteSpace(std::string &content)
{
	size_t	i = 0;

	while (content[i] && isspace(content[i]))
		i++;
	content = content.substr(i);
	i = content.length() - 1;
	while (i > 0 && isspace(content[i]))
		i--;
	content = content.substr(0, i + 1);
}


/* finding a server begin and return the index of { start of server */
size_t ConfigFileParser::findStartServer(size_t start, std::string &content)
{
	size_t i;

	for (i = start; content[i]; i++)
	{
		if (content[i] == 's')
			break ;
		if (!isspace(content[i]))
			("Wrong character out of server scope{}");
	}
	if (!content[i])
		return (start);
	if (content.compare(i, 6, "server") != 0)
		("Wrong character out of server scope{}");
	i += 6;
	while (content[i] && isspace(content[i]))
		i++;
	if (content[i] == '{')
		return (i);
	else
		("Wrong character out of server scope{}");

}

/* finding a server end and return the index of } end of server */
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

/* spliting line by separator */
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

/* spliting servers on separetly strings in vector */
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
			std::cerr << "problem with scope" << std::endl;
		this->_serversConfig.push_back(content.substr(start, end - start + 1));
		this->_numOfServers++;
		start = end + 1;
	}
}

/* creating Server from string and fill the value */
void ConfigFileParser::createServer(std::string &config, ServerConfig &server)
{
	std::vector<std::string>	parametrs;
	std::vector<std::string>	error_codes;
	int		flag_loc = 1;
	bool	flag_autoindex = false;
	bool	flag_max_size = false;

	parametrs = splitParametrs(config += ' ', std::string(" \n\t"));
	if (parametrs.size() < 3)
		("Failed server validation");
	for (size_t i = 0; i < parametrs.size(); i++)
	{
		if (parametrs[i] == "listen" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (server.getPort())
				("Port is duplicated");
			server.setPort(parametrs[++i]);
		}
		else if (parametrs[i] == "location" && (i + 1) < parametrs.size())
		{
			std::string	path;
			i++;
			if (parametrs[i] == "{" || parametrs[i] == "}")
				("Wrong character in server scope{}");
			path = parametrs[i];
			std::vector<std::string> codes;
			if (parametrs[++i] != "{")
				("Wrong character in server scope{}");
			i++;
			while (i < parametrs.size() && parametrs[i] != "}")
				codes.push_back(parametrs[i++]);
			server.setLocation(path, codes);
			if (i < parametrs.size() && parametrs[i] != "}")
				("Wrong character in server scope{}");
			flag_loc = 0;
		}
		else if (parametrs[i] == "host" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (server.getHost())
				("Host is duplicated");
			server.setHost(parametrs[++i]);
		}
		else if (parametrs[i] == "root" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (!server.getRoot().empty())
				("Root is duplicated");
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
					("Wrong character out of server scope{}");
			}
		}
		else if (parametrs[i] == "client_max_body_size" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (flag_max_size)
				("Client_max_body_size is duplicated");
			server.setClientMaxBodySize(parametrs[++i]);
			flag_max_size = true;
		}
		else if (parametrs[i] == "server_name" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (!server.getServerName().empty())
				("Server_name is duplicated");
			server.setServerName(parametrs[++i]);
		}
		else if (parametrs[i] == "index" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (!server.getIndex().empty())
				("Index is duplicated");
			server.setIndex(parametrs[++i]);
		}
		else if (parametrs[i] == "autoindex" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (flag_autoindex)
				("Autoindex of server is duplicated");
			server.setAutoindex(parametrs[++i]);
			flag_autoindex = true;
		}
		else if (parametrs[i] != "}" && parametrs[i] != "{")
		{
			if (!flag_loc)
				("Parametrs after location");
			else
				("Unsupported directive");
		}
	}
	if (server.getRoot().empty())
		server.setRoot("/;");
	if (server.getHost() == 0)
		server.setHost("localhost;");
	if (server.getIndex().empty())
		server.setIndex("index.html;");
	if (ConfigFile::checkFile(server.getRoot(), server.getIndex()))
		("Index from config file not found or unreadable");
	if (server.checkLocaitons())
		("Locaition is duplicated");
	if (!server.getPort())
		("Port not found");
	server.setErrorPages(error_codes);
	if (!server.isValidErrorPages())
		("Incorrect path for error page or number of error");
}