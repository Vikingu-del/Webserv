/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 15:03:51 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/28 13:11:59 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

ServerConfig::ServerConfig()
	:_port(0),
	_host(0),
	_server_name(""),
	_root(""),
	_client_max_body_size(MAX_CONTENT_LENGTH),
	_index(""),
	_autoindex(false),
	_locations(0),
	_listen_fd(0)
{	
	// std::cout << PINK BLD "ServerConfig default constructor" RST << std::endl;
	initErrorPages();
}

ServerConfig::ServerConfig(const ServerConfig &other)
	:_port(other._port),
	_host(other._host),
	_server_name(other._server_name),
	_root(other._root),
	_client_max_body_size(other._client_max_body_size),
	_index(other._index),
	_autoindex(other._autoindex),
	_locations(other._locations),
	_listen_fd(other._listen_fd),
	_error_pages(other._error_pages)
{
	// std::cout << PINK BLD "ServerConfig copy constructor" RST << std::endl;
}

ServerConfig &ServerConfig::operator=(const ServerConfig &other)
{
	// std::cout << PINK BLD "ServerConfig assignation operator" RST << std::endl;
	if (this != &other)
	{
		this->_server_name = other._server_name;
		this->_root = other._root;
		this->_port = other._port;
		this->_host = other._host;
		this->_client_max_body_size = other._client_max_body_size;
		this->_index = other._index;
		this->_error_pages = other._error_pages;
		this->_locations = other._locations;
		this->_listen_fd = other._listen_fd;
		this->_autoindex = other._autoindex;
		this->_server_address = other._server_address;
	}
	return (*this);
}

// !Destructor

ServerConfig::~ServerConfig(){
	// std::cout << RED BLD "ServerConfig destructor" RST << std::endl;
	// ? no specific cleanup needed
	
}

// !Getters

const uint16_t &ServerConfig::getPort()
{
	// std::cout << PINK BLD "ServerConfig getPort called" RST << std::endl;
	return (this->_port);
}

const in_addr_t &ServerConfig::getHost()
{
	// std::cout << PINK BLD "ServerConfig getHost called" RST << std::endl;
	return (this->_host);
}

const std::string &ServerConfig::getServerName()
{
	// std::cout << PINK BLD "ServerConfig getServerName called" RST << std::endl;
	return (this->_server_name);
}

const std::string &ServerConfig::getRoot()
{
	// std::cout << PINK BLD "ServerConfig getRoot called" RST << std::endl;
	return (this->_root);
}

const unsigned long &ServerConfig::getClientMaxBody()
{
	// std::cout << PINK BLD "ServerConfig getClientMaxBody called" RST << std::endl;
	return (this->_client_max_body_size);
}

const std::string &ServerConfig::getIndex()
{
	// std::cout << PINK BLD "ServerConfig getIndex called" RST << std::endl;
	return (this->_index);
}

const bool & ServerConfig::getAutoindex()
{
	// std::cout << PINK BLD "ServerConfig getAutoindex called" RST << std::endl;
	return (this->_autoindex);
}

const std::map<short, std::string> &ServerConfig::getErrorPages()
{
	// std::cout << PINK BLD "ServerConfig getErrorPages called" RST << std::endl;
	return (this->_error_pages);
}

const std::vector<Location> &ServerConfig::getLocations() {
	// std::cout << PINK BLD "ServerConfig getLocations called" RST << std::endl;
	return (this->_locations);
}

const sockaddr_in &ServerConfig::getServerArddres()
{
	// std::cout << PINK BLD "ServerConfig getServerArddres called" RST << std::endl;
	return (this->_server_address);
}

const int &ServerConfig::getListenFd()
{
	// std::cout << PINK BLD "ServerConfig getListenFd called" RST << std::endl;
	return(this->_listen_fd);
}


// !Setters

void ServerConfig::setServerName(std::string server_name)
{
	// std::cout << PINK BLD "ServerConfig setServerName called" RST << std::endl;
	checkToken(server_name);
	this->_server_name = server_name;
}

void ServerConfig::setHost(std::string parametr)
{
	// std::cout << PINK BLD "ServerConfig setHost called" RST << std::endl;
	checkToken(parametr);
	if (parametr == "localhost")
		parametr = "127.0.0.1";
	if (!isValidHost(parametr))
		throw ServerConfigException("Wrong syntax: host");
	this->_host = inet_addr(parametr.data());
}

/* validation of parametrs */
bool ServerConfig::isValidHost(std::string host) const
{
	// std::cout << PINK BLD "ServerConfig isValidHost called" RST << std::endl;
	struct sockaddr_in sockaddr;
  	return (inet_pton(AF_INET, host.c_str(), &(sockaddr.sin_addr)) ? true : false);
}


void ServerConfig::setRoot(std::string root)
{
	// std::cout << PINK BLD "ServerConfig setRoot called" RST << std::endl;
	checkToken(root);
	if (ConfigFile::checkFileExistence(root) == 2)
	{
		this->_root = root;
		return ;
	}
	char dir[1024];
	getcwd(dir, 1024);
	std::string full_root = dir + root;
	if (ConfigFile::checkFileExistence(full_root) != 2)
		throw ServerConfigException("Root directory does not exist: " + full_root);
	this->_root = full_root;
}

void ServerConfig::setPort(std::string parametr)
{
	// std::cout << PINK BLD "ServerConfig setPort called" RST << std::endl;
	unsigned int port;
	port = 0;
	checkToken(parametr);
	for (size_t i = 0; i < parametr.length() ; i++)
	{
		if (!std::isdigit(parametr[i]))
			throw ServerConfigException("Wrong syntax: port");
	}
	port = utils::strToInt((parametr));
	if (port < 1 || port > 65636)
		throw ServerConfigException("Wrong syntax: port");
	this->_port = (uint16_t) port;
}

void ServerConfig::setClientMaxBodySize(std::string parametr)
{
	// std::cout << PINK BLD "ServerConfig setClientMaxBodySize called" RST << std::endl;
	unsigned long body_size;
	
	body_size = 0;
	checkToken(parametr);
	for (size_t i = 0; i < parametr.length(); i++)
	{
		if (parametr[i] < '0' || parametr[i] > '9')
			throw ServerConfigException("Wrong syntax: client_max_body_size");
	}
	if (!utils::strToInt(parametr))
		throw ServerConfigException("Wrong syntax: client_max_body_size");
	body_size = utils::strToInt(parametr);
	this->_client_max_body_size = body_size;
}

void ServerConfig::setIndex(std::string index)
{
	// std::cout << PINK BLD "ServerConfig setIndex called" RST << std::endl;
	checkToken(index);
	this->_index = index;
}

void ServerConfig::setAutoindex(std::string autoindex)
{
	// std::cout << PINK BLD "ServerConfig setAutoindex called" RST << std::endl;
	checkToken(autoindex);
	if (autoindex != "on" && autoindex != "off")
		throw ServerConfigException("Wrong syntax: autoindex");
	if (autoindex == "on")
		this->_autoindex = true;
}

/* checks if there is such a default error code. If there is, it overwrites the path to the file,
otherwise it creates a new pair: error code - path to the file */
void ServerConfig::setErrorPages(const std::vector<std::string> &parametr)
{
	// std::cout << PINK BLD "ServerConfig setErrorPages called" RST << std::endl;
	if (parametr.empty())
		return;
	if (parametr.size() % 2 != 0)
		throw ServerConfigException("Error page initialization failed");
	for (size_t i = 0; i < parametr.size() - 1; i++)
	{
		for (size_t j = 0; j < parametr[i].size(); j++) {
			if (!std::isdigit(parametr[i][j]))
				throw ServerConfigException("Error code is invalid");
		}
		if (parametr[i].size() != 3)
			throw ServerConfigException("Error code is invalid");
		short code_error = utils::strToInt(parametr[i]);
		if (utils::statusCodeString(code_error)  == "Undefined" || code_error < 400)
			throw ServerConfigException("Incorrect error co : " + parametr[i]);
		i++;
		std::string path = parametr[i];
		checkToken(path);
		if (ConfigFile::checkFileExistence(path) != 2)
		{
			if (ConfigFile::checkFileExistence(this->_root + path) != 1)
				throw ServerConfigException("Incorrect path for error page file: " + this->_root + path); //TODO: CHECK THIS ONE MOREW TIME NO PERSSIONS
			if (ConfigFile::checkFile(this->_root + path, NO_PERMISSIONS) == -1 || ConfigFile::checkFilePermissons(this->_root + path, R_OK) == -1)
				throw ServerConfigException("Error page file is not accessible: " + this->_root + path); //TODO: CHECK THIS ONE MOREW TIME NO PERSSIONS
		}
		std::map<short, std::string>::iterator it = this->_error_pages.find(code_error);
		if (it != _error_pages.end())
			this->_error_pages[code_error] = path;
		else
			this->_error_pages.insert(std::make_pair(code_error, path));
	}
}

/* parsing and set locations */
void ServerConfig::setLocation(std::string path, std::vector<std::string> parametr)
{
	// std::cout << PINK BLD "ServerConfig setLocation called" RST << std::endl;
	Location					new_location;
	std::vector<std::string>	methods;
	bool						flag_methods = false;
	bool						flag_autoindex = false;
	bool						flag_max_size = false;
	int							valid;

	new_location.setPath(path);
	for (size_t i = 0; i < parametr.size(); i++)
	{
		if (parametr[i] == "root" && (i + 1) < parametr.size())
		{
			if (!new_location.getRootLocation().empty())
				throw ServerConfigException("Root of location  duplicated");
			checkToken(parametr[++i]);
			if (ConfigFile::checkFileExistence(parametr[i]) == 2)
				new_location.setRootLocation(parametr[i]);
			else
				new_location.setRootLocation(this->_root + parametr[i]);
		}
		else if ((parametr[i] == "allow_methods" || parametr[i] == "methods") && (i + 1) < parametr.size())
		{
			if (flag_methods)
				throw ServerConfigException("Allow_methods of location duplicated");
			std::vector<std::string> methods;
			while (++i < parametr.size())
			{
				if (parametr[i].find(";") != std::string::npos)
				{
					checkToken(parametr[i]);
					methods.push_back(parametr[i]);
					break ;
				}
				else
				{
					methods.push_back(parametr[i]);
					if (i + 1 >= parametr.size())
						throw ServerConfigException("Token is invalid");
				}
			}
			new_location.setMethods(methods);
			flag_methods = true;
		}
		else if (parametr[i] == "autoindex" && (i + 1) < parametr.size())
		{
			if (path == "/cgi-bin")
				throw ServerConfigException("Parametr autoindex not all for CGI");

			if (flag_autoindex)
				throw ServerConfigException("Autoindex of location  duplicated");

			checkToken(parametr[++i]);
			new_location.setAutoindex(parametr[i]);
			flag_autoindex = true;
		}
		else if (parametr[i] == "index" && (i + 1) < parametr.size())
		{
			if (!new_location.getIndexLocation().empty())
				throw ServerConfigException("Index of location  duplicated");
			checkToken(parametr[++i]);
			new_location.setIndexLocation(parametr[i]);
		}
		else if (parametr[i] == "return" && (i + 1) < parametr.size())
		{
			if (path == "/cgi-bin")
				throw ServerConfigException("Parametr return not all for CGI");
			if (!new_location.getReturn().empty())
				throw ServerConfigException("Return of location duplicated");
			checkToken(parametr[++i]);
			new_location.setReturn(parametr[i]);
		}
		else if (parametr[i] == "alias" && (i + 1) < parametr.size())
		{
			if (path == "/cgi-bin")
				throw ServerConfigException("Parametr alias not all for CGI");
			if (!new_location.getAlias().empty())
				throw ServerConfigException("Alias of location duplicated");
			checkToken(parametr[++i]);
			new_location.setAlias(parametr[i]);
		}
		else if (parametr[i] == "cgi_ext" && (i + 1) < parametr.size())
		{
			std::vector<std::string> extension;
			while (++i < parametr.size())
			{
				if (parametr[i].find(";") != std::string::npos)
				{
					checkToken(parametr[i]);
					extension.push_back(parametr[i]);
					break ;
				}
				else
				{
					extension.push_back(parametr[i]);
					if (i + 1 >= parametr.size())
						throw ServerConfigException("Token is invalid");
				}
			}
			new_location.setCgiExtension(extension);
		}
		else if (parametr[i] == "cgi_path" && (i + 1) < parametr.size())
		{
			std::vector<std::string> path;
			while (++i < parametr.size())
			{
				if (parametr[i].find(";") != std::string::npos)
				{
					checkToken(parametr[i]);
					path.push_back(parametr[i]);
					break ;
				}
				else
				{
					path.push_back(parametr[i]);
					if (i + 1 >= parametr.size())
						throw ServerConfigException("Token is invalid");
				}
				if (parametr[i].find("/python") == std::string::npos && parametr[i].find("/bash") == std::string::npos)	
					throw ServerConfigException("cgi_path is invalid");
			}
			new_location.setCgiPath(path);
		}
		else if (parametr[i] == "client_max_body_size" && (i + 1) < parametr.size())
		{
			if (flag_max_size)
				throw ServerConfigException("Maxbody_size of location duplicated");
			checkToken(parametr[++i]);
			new_location.setMaxBodySize(parametr[i]);
			flag_max_size = true;
		}
		else if (i < parametr.size())
			throw ServerConfigException("Parametr in a locati is invalid");
	}
	if (new_location.getPath() != "/cgi-bin" && new_location.getIndexLocation().empty())
		new_location.setIndexLocation(this->_index);
	if (!flag_max_size)
		new_location.setMaxBodySize(this->_client_max_body_size);
	valid = isValidLocation(new_location);
	if (valid == 1)
		throw ServerConfigException("Failed CGI validation");
	if (valid == 2)
		throw ServerConfigException("Failed path in locatio validation");
	else if (valid == 3)
		throw ServerConfigException("Failed return file location validation");
	else if (valid == 4)
		throw ServerConfigException("Failed alias file location validation");
	this->_locations.push_back(new_location);
}

void ServerConfig::setFd(int fd)
{
	this->_listen_fd = fd;
}


// !Init Error Pages 

void ServerConfig::initErrorPages(void)
{
	_error_pages[301] = "";
	_error_pages[302] = "";
	_error_pages[400] = "";
	_error_pages[401] = "";
	_error_pages[402] = "";
	_error_pages[403] = "";
	_error_pages[404] = "";
	_error_pages[405] = "";
	_error_pages[406] = "";
	_error_pages[500] = "";
	_error_pages[501] = "";
	_error_pages[502] = "";
	_error_pages[503] = "";
	_error_pages[505] = "";
	_error_pages[505] = "";
}

/* check parametrs of location */
int ServerConfig::isValidLocation(Location & location) const
{
	// std::cout << PINK BLD "ServerConfig isValidLocation called" RST << std::endl;
	if (location.getPath() == "/cgi-bin")
	{
		if (location.getCgiPath().empty() || location.getCgiExtension().empty() || location.getIndexLocation().empty())
			return (1);
		
		if (ConfigFile::checkFilePermissons(location.getIndexLocation(), 4) < 0)
		{
			std::string path = location.getRootLocation() + location.getPath() + "/" + location.getIndexLocation();
			if (ConfigFile::checkFileExistence(path) != 1)
			{				
				std::string root = getcwd(NULL, 0);
				location.setRootLocation(root);
				path = root + location.getPath() + "/" + location.getIndexLocation();
			}
			if (path.empty() || ConfigFile::checkFileExistence(path) != 1 || ConfigFile::checkFilePermissons(path, 4) < 0)
				return (1);
		}
		if (location.getCgiPath().size() != location.getCgiExtension().size())
			return (1);
		std::vector<std::string>::const_iterator it;
		for (it = location.getCgiPath().begin(); it != location.getCgiPath().end(); ++it)
		{
			if (ConfigFile::checkFileExistence(*it) < 0)
				return (1);
		}
		
		std::vector<std::string>::const_iterator it_path;
		
		for (it = location.getCgiExtension().begin(); it != location.getCgiExtension().end(); ++it)
		{
			std::string tmp = *it;
			if (tmp != ".py" && tmp != ".sh" && tmp != "*.py" && tmp != "*.sh")
				return (1);
			for (it_path = location.getCgiPath().begin(); it_path != location.getCgiPath().end(); ++it_path)
			{
				std::string tmp_path = *it_path;
				if (tmp == ".py" || tmp == "*.py")
				{
					if (tmp_path.find("python") != std::string::npos)
						location._ext_path.insert(std::make_pair(".py", tmp_path));
				}
				else if (tmp == ".sh" || tmp == "*.sh")
				{
					if (tmp_path.find("bash") != std::string::npos)
						location._ext_path[".sh"] = tmp_path;
				}
			}
		}
		if (location.getCgiPath().size() != location.getExtensionPath().size())
			return (1);
	}
	else
	{
		if (location.getPath()[0] != '/')
			return (2);
		if (location.getRootLocation().empty()) {
			location.setRootLocation(this->_root);
		}
		if (ConfigFile::checkFile(location.getRootLocation() + location.getPath() + "/", location.getIndexLocation()))
			return (5);
		if (!location.getReturn().empty())
		{
			if (ConfigFile::checkFile(location.getRootLocation(), location.getReturn()))
				return (3);
		}
		if (!location.getAlias().empty())
		{
			if (ConfigFile::checkFile(location.getRootLocation(), location.getAlias()))
			 	return (4);
		}
	}
	return (0);
}


bool ServerConfig::isValidErrorPages()
{
	// std::cout << PINK BLD "ServerConfig isValidErrorPages called" RST << std::endl;
	std::map<short, std::string>::const_iterator it;
	for (it = this->_error_pages.begin(); it != this->_error_pages.end(); it++)
	{
		if (it->first < 100 || it->first > 599)
			return (false);
		if (ConfigFile::checkFilePermissons(getRoot() + it->second, 0) < 0 || ConfigFile::checkFilePermissons(getRoot() + it->second, 4) < 0)
			return (false);
	}
	return (true);
}

//! Check that Token is properly formatted
void ServerConfig::checkToken(std::string &parametr)
{
	size_t pos = parametr.rfind(';');
	if (pos != parametr.size() - 1)
		throw ServerConfigException("Token is invalid");
	parametr.erase(pos);
}

//! Check that locations not duplicated
bool ServerConfig::checkLocaitons() const
{
	// std::cout << PINK BLD "ServerConfig checkLocaitons called" RST << std::endl;
	if (this->_locations.size() < 2)
		return (false);
	std::vector<Location>::const_iterator it1;
	std::vector<Location>::const_iterator it2;
	for (it1 = this->_locations.begin(); it1 != this->_locations.end() - 1; it1++) {
		for (it2 = it1 + 1; it2 != this->_locations.end(); it2++) {
			if (it1->getPath() == it2->getPath())
				return (true);
		}
	}
	return (false);
}

//! Bind server
void	ServerConfig::bindServer(void)
{
	// std::cout << PINK BLD "ServerConfig bindServer called" RST << std::endl;
	if ((_listen_fd = socket(AF_INET, SOCK_STREAM, 0) )  == -1 )
	{
		// Logger::logMsg(RED, CONSOLE_OUTPUT, "webserv: socket error %s   Closing ....", strerror(errno));
		// exit(EXIT_FAILURE);
		std::cerr << "ERR BINDING SOCKET" << std::endl;
		exit(1);
	}

	int option_value = 1;
	if (setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(int)) == -1)
		throw ServerConfigException("setsockopt error");
	memset(&_server_address, 0, sizeof(_server_address));
	_server_address.sin_family = AF_INET;
	_server_address.sin_addr.s_addr = _host;
	_server_address.sin_port = htons(_port);
	if (bind(_listen_fd, (struct sockaddr *) &_server_address, sizeof(_server_address)) == -1)
	{
		// Logger::logMsg(RED, CONSOLE_OUTPUT, "webserv: bind error %s   Closing ....", strerror(errno));
		// exit(EXIT_FAILURE);
		std::cerr << "ERR BINDING SOCKET" << std::endl;
		exit(1);
	}
}
