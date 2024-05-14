/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfiguration.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 15:03:51 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/10 18:04:24 by ipetruni         ###   ########.fr       */
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