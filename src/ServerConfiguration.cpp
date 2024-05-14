/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfiguration.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 15:03:51 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/14 18:45:48 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ServerConfiguration.hpp"

// !Constructors

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
}

ServerConfig &ServerConfig::operator=(const ServerConfig &other)
{
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
	
}

// !Getters

const uint16_t &ServerConfig::getPort()
{
	return (this->_port);
}

const in_addr_t &ServerConfig::getHost()
{
	return (this->_host);
}

const std::string &ServerConfig::getServerName()
{
	return (this->_server_name);
}

const std::string &ServerConfig::getRoot()
{
	return (this->_root);
}

const unsigned long &ServerConfig::getClientMaxBody()
{
	return (this->_client_max_body_size);
}

const std::string &ServerConfig::getIndex()
{
	return (this->_index);
}

const bool & ServerConfig::getAutoindex()
{
	return (this->_autoindex);
}

const std::map<short, std::string> &ServerConfig::getErrorPages()
{
	return (this->_error_pages);
}

const std::vector<Location> &ServerConfig::getLocations() {
	return (this->_locations);
}

const sockaddr_in &ServerConfig::getServerArddres()
{
	return (this->_server_address);
}

const int &ServerConfig::getListenFd()
{
	return(this->_listen_fd);
}


// !Setters

void ServerConfig::setServerName(std::string server_name)
{
	checkToken(server_name);
	this->_server_name = server_name;
}

void ServerConfig::setHost(std::string parametr)
{
	checkToken(parametr);
	if (parametr == "localhost")
		parametr = "127.0.0.1";
	if (!isValidHost(parametr))
		std::cerr << "Wrong syntax: host" << std::endl;
	this->_host = inet_addr(parametr.data());
}

/* validation of parametrs */
bool ServerConfig::isValidHost(std::string host) const
{
	struct sockaddr_in sockaddr;
  	return (inet_pton(AF_INET, host.c_str(), &(sockaddr.sin_addr)) ? true : false);
}


void ServerConfig::setRoot(std::string root)
{
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
		std::cerr << "Wrong syntax: root" << std::endl;
	this->_root = full_root;
}

void ServerConfig::setPort(std::string parametr)
{
	unsigned int port;
	
	port = 0;
	checkToken(parametr);
	for (size_t i = 0; i < parametr.length(); i++)
	{
		if (!std::isdigit(parametr[i]))
			std::cerr << "Wrong syntax: port" << std::endl;
	}
	port = ft_stoi((parametr));
	if (port < 1 || port > 65636)
		std::cerr << "Wrong syntax: port" << std::endl;
	this->_port = (uint16_t) port;
}

void ServerConfig::setClientMaxBodySize(std::string parametr)
{
	unsigned long body_size;
	
	body_size = 0;
	checkToken(parametr);
	for (size_t i = 0; i < parametr.length(); i++)
	{
		if (parametr[i] < '0' || parametr[i] > '9')
			std::cerr << "Wrong syntax: client_max_body_size" << std::endl;
	}
	if (!ft_stoi(parametr))
		std::cerr << "Wrong syntax: client_max_body_size" << std::endl;
	body_size = ft_stoi(parametr);
	this->_client_max_body_size = body_size;
}

void ServerConfig::setIndex(std::string index)
{
	checkToken(index);
	this->_index = index;
}

void ServerConfig::setAutoindex(std::string autoindex)
{
	checkToken(autoindex);
	if (autoindex != "on" && autoindex != "off")
		std::cerr << "Wrong syntax: autoindex" << std::endl;
	if (autoindex == "on")
		this->_autoindex = true;
}

/* checks if there is such a default error code. If there is, it overwrites the path to the file,
otherwise it creates a new pair: error code - path to the file */
void ServerConfig::setErrorPages(std::vector<std::string> &parametr)
{
	if (parametr.empty())
		return;
	if (parametr.size() % 2 != 0)
		std::cerr << "Error page initialization faled" << std::endl;
	for (size_t i = 0; i < parametr.size() - 1; i++)
	{
		for (size_t j = 0; j < parametr[i].size(); j++) {
			if (!std::isdigit(parametr[i][j]))
				std::cerr << "Error code is invalid" << std::endl;
		}
		if (parametr[i].size() != 3)
			std::cerr << "Error code is invalid" << std::endl;
		short code_error = ft_stoi(parametr[i]);
		if (statusCodeString(code_error)  == "Undefined" || code_error < 400)
			std::cerr << ("Incorrect error co : " + parametr[i]) << std::endl;
		i++;
		std::string path = parametr[i];
		checkToken(path);
		if (ConfigFile::checkFileExistence(path) != 2)
		{
			if (ConfigFile::checkFileExistence(this->_root + path) != 1)
				std::cerr << ("Incorrect path for err << std::endl; page file: " + this->_root + path);
			if (ConfigFile::checkFile(this->_root + path, 0) == -1 || ConfigFile::checkFilePermissons(this->_root + path, 4) == -1)
				std::cerr << "Error page file" <<  this->_root + path << "is not accessible" << std::endl;
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
	Location new_location;
	std::vector<std::string> methods;
	bool flag_methods = false;
	bool flag_autoindex = false;
	bool flag_max_size = false;
	int valid;

	new_location.setPath(path);
	for (size_t i = 0; i < parametr.size(); i++)
	{
		if (parametr[i] == "root" && (i + 1) < parametr.size())
		{
			if (!new_location.getRootLocation().empty())
				std::cerr << "Root of location  << std::endl; duplicated";
			checkToken(parametr[++i]);
			if (ConfigFile::checkFileExistence(parametr[i]) == 2)
				new_location.setRootLocation(parametr[i]);
			else
				new_location.setRootLocation(this->_root + parametr[i]);
		}
		else if ((parametr[i] == "allow_methods" || parametr[i] == "methods") && (i + 1) < parametr.size())
		{
			if (flag_methods)
				std::cerr << "Allow_methods of location duplicated" << std::endl;
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
						std::cerr << "Token is invalid" << std::endl;
				}
			}
			new_location.setMethods(methods);
			flag_methods = true;
		}
		else if (parametr[i] == "autoindex" && (i + 1) < parametr.size())
		{
			if (path == "/cgi-bin")
				std::cerr << "Parametr autoindex not all for CGI" << std::endl;

			if (flag_autoindex)
				std::cerr << "Autoindex of location  duplicated" << std::endl;

			checkToken(parametr[++i]);
			new_location.setAutoindex(parametr[i]);
			flag_autoindex = true;
		}
		else if (parametr[i] == "index" && (i + 1) < parametr.size())
		{
			if (!new_location.getIndexLocation().empty())
				std::cerr << "Index of location  duplicated" << std::endl;
			checkToken(parametr[++i]);
			new_location.setIndexLocation(parametr[i]);
		}
		else if (parametr[i] == "return" && (i + 1) < parametr.size())
		{
			if (path == "/cgi-bin")
				std::cerr << "Parametr return not all << std::endl; for CGI";
			if (!new_location.getReturn().empty())
				std::cerr << "Return of location  << std::endl; duplicated";
			checkToken(parametr[++i]);
			new_location.setReturn(parametr[i]);
		}
		else if (parametr[i] == "alias" && (i + 1) < parametr.size())
		{
			if (path == "/cgi-bin")
				std::cerr << "Parametr alias not all << std::endl; for CGI";
			if (!new_location.getAlias().empty())
				std::cerr << "Alias of location  << std::endl; duplicated";
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
						std::cerr << "Token is invalid" << std::endl;
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
						std::cerr << "Token is invalid" << std::endl;
				}
				if (parametr[i].find("/python") == std::string::npos && parametr[i].find("/bash") == std::string::npos)	
					std::cerr << "cgi_path is invalid" << std::endl;
				}
			new_location.setCgiPath(path);
		}
		else if (parametr[i] == "client_max_body_size" && (i + 1) < parametr.size())
		{
			if (flag_max_size)
				std::cerr << "Maxbody_size of location duplicated" << std::endl;
			checkToken(parametr[++i]);
			new_location.setMaxBodySize(parametr[i]);
			flag_max_size = true;
		}
		else if (i < parametr.size())
			std::cerr << "Parametr in a locati is invalid" << std::endl;
	}
	if (new_location.getPath() != "/cgi-bin" && new_location.getIndexLocation().empty())
		new_location.setIndexLocation(this->_index);
	if (!flag_max_size)
		new_location.setMaxBodySize(this->_client_max_body_size);
	valid = isValidLocation(new_location);
	if (valid == 1)
		std::cerr << "Failed CGI validation" << std::endl;
	if (valid == 2)
		std::cerr << "Failed path in locaiti validation" << std::endl;
	else if (valid == 3)
		std::cerr << "Failed redirection file locaition validation" << std::endl;
	else if (valid == 4)
		std::cerr << "Failed alias file locaition validation" << std::endl;
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

/* check is a properly end of parametr */
void ServerConfig::checkToken(std::string &parametr)
{
	size_t pos = parametr.rfind(';');
	if (pos != parametr.size() - 1)
		std::cerr << "Token is invalid" << std::endl;
	parametr.erase(pos);
}


/* check parametrs of location */
int ServerConfig::isValidLocation(Location & location) const
{
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

/* socket setup and binding */
void	ServerConfig::bindServer(void)
{
	if ((_listen_fd = socket(AF_INET, SOCK_STREAM, 0) )  == -1 )
    {
		Logger::logMsg(RED, CONSOLE_OUTPUT, "webserv: socket error %s   Closing ....", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int option_value = 1;
    setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(int));
    memset(&_server_address, 0, sizeof(_server_address));
    _server_address.sin_family = AF_INET;
    _server_address.sin_addr.s_addr = _host;
    _server_address.sin_port = htons(_port);
    if (bind(_listen_fd, (struct sockaddr *) &_server_address, sizeof(_server_address)) == -1)
    {
		Logger::logMsg(RED, CONSOLE_OUTPUT, "webserv: bind error %s   Closing ....", strerror(errno));
        exit(EXIT_FAILURE);
    }
}