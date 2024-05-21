/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 16:11:16 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/20 13:17:14 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

// !Constructors

Location::Location()
	:_path(""),
	_root(""),
	_autoindex(false),
	_index(""),
	_return(""),
	_alias(""),
	_client_max_body_size(MAX_CONTENT_LENGTH)
{
	_methods.push_back(1);
    _methods.push_back(0);
    _methods.push_back(0);
    _methods.push_back(0);
    _methods.push_back(0);
}

Location::Location(const Location &other)
	: _path(other._path),
      _root(other._root),
      _autoindex(other._autoindex),
      _index(other._index),
      _return(other._return),
      _alias(other._alias),
      _client_max_body_size(other._client_max_body_size),
      _methods(other._methods)
{
	// std::cout << BLUE << "Location copy constructor" << RST << std::endl;
}

Location &Location::operator=(const Location &other) {
	// std::cout << BLUE << "Location assignation operator" << RST << std::endl;
	if (this != &other)
	{
		_path = other._path;
		_root = other._root;
		_autoindex = other._autoindex;
		_index = other._index;
		_return = other._return;
		_alias = other._alias;
		_client_max_body_size = other._client_max_body_size;
		_methods = other._methods;
	}
	return *this;
}


// !Destructor

Location::~Location() {
	// std::cout << RED << "Location destructor" << RST << std::endl;
	// ? no specific cleanup needed
}


// !Getters

const std::string &Location::getPath() const {
	// std::cout << BLUE "Location getPath called" RST << std::endl;
	return(this->_path);
}

const std::string &Location::getRootLocation() const {
	// std::cout << BLUE "Location getRootLocation called" RST << std::endl;
	return(this->_root);
}

const bool &Location::getAutoindex() const {
	// std::cout << BLUE "Location getAutoindex called" RST << std::endl;
	return(this->_autoindex);
}

const std::string &Location::getIndexLocation() const {
	// std::cout << BLUE "Location getIndexLocation called" RST << std::endl;
	return(this->_index);
}

const std::vector<short> &Location::getMethods() const {
	// std::cout << BLUE "Location getMethods called" RST << std::endl;
	return(this->_methods);
}

const std::string &Location::getReturn() const {
	// std::cout << BLUE "Location getReturn called" RST << std::endl;
	return(this->_return);
}

const std::string &Location::getAlias() const {
	// std::cout << BLUE "Location getAlias called" RST << std::endl;
	return(this->_alias);
}

const std::vector<std::string> &Location::getCgiPath() const {
	// std::cout << BLUE "Location getCgiPath called" RST << std::endl;
	return (this->_cgi_path);
}

const std::vector<std::string> &Location::getCgiExtension() const {
	// std::cout << BLUE "Location getCgiExtension called" RST << std::endl;
	return (this->_cgi_ext);
}

const std::map<std::string, std::string> &Location::getExtensionPath() const {
	// std::cout << BLUE "Location getExtensionPath called" RST << std::endl;
	return(this->_ext_path);
}

const unsigned long &Location::getMaxBodySize() const {
	// std::cout << BLUE "Location getMaxBodySize called" RST << std::endl;
	return(this->_client_max_body_size);
}

// !Setters

void Location::setPath(std::string value) {
	// std::cout << BLUE "Location setPath called" RST << std::endl;
	this->_path = value;
}

void Location::setRootLocation(std::string value) {
	// std::cout << BLUE "Location setRootLocation called" RST << std::endl;
	if (ConfigFile::checkFileExistence(value) != 2)
		std::cerr << "Root of location" << std::endl; // TODO: Exception Class in ServerConfig
	this->_root = value;
}

void Location::setMethods(std::vector<std::string> methods) {
	// std::cout << BLUE "Location setMethods called" RST << std::endl;
	// !Init all elements of _methods to 0.
	std::fill(this->_methods.begin(), this->_methods.end(), 0);
	
	for(size_t i = 0; i < methods.size(); i++)
	{
		if (methods[i] == "GET")
			this->_methods[0] = 1;
		else if (methods[i] == "POST")
			this->_methods[0] = 1;
		else if (methods[i] == "DELETE")
			this->_methods[0] = 1;
		else if (methods[i] == "PUT")
			this->_methods[0] = 1;
		else if (methods[i] == "HEAD")
			this->_methods[0] = 1;
		else
			std::cerr << "Allow method not supported " << methods[i] << std::endl; // TODO: Exception Class in ServerConfig
	}
}

void Location::setAutoindex(std::string value) {
	// std::cout << BLUE "Location setAutoindex called" RST << std::endl;
	if (value == "on" || value == "off")
		this->_autoindex = (value == "on");
	else
		std::cerr << "Wrong autoindex" << std::endl; // TODO: Exception Class in ServerConfig
}

void Location::setIndexLocation(std::string value) {
	// std::cout << BLUE "Location setIndexLocation called" RST << std::endl;
	this->_index = value;
}

void Location::setReturn(std::string value) {
	// std::cout << BLUE "Location setReturn called" RST << std::endl;
	this->_return = value;
}

void Location::setAlias(std::string value) {
	// std::cout << BLUE "Location setAlias called" RST << std::endl;
	this->_alias = value;
}

void Location::setCgiPath(std::vector<std::string> path) {
	// std::cout << BLUE "Location setCgiPath called" RST << std::endl;
	this->_cgi_path = path;
}

void Location::setCgiExtension(std::vector<std::string> extension) {
	// std::cout << BLUE "Location setCgiExtension called" RST << std::endl;
	this->_cgi_ext = extension;
}

void Location::setMaxBodySize(std::string string_value) {
	// std::cout << BLUE "Location setMaxBodySize called" RST << std::endl;
	unsigned long body_size = 0;

	for (size_t i = 0; i < string_value.length(); i++) {
		if (string_value[i] < '0' || string_value[i] > '9')
			std::cerr << "Wrong syntax: client_max_body_size" << std::endl; // TODO: Exception Class in ServerConfig
	}
	if (!ft_stoi(string_value))
		std::cerr << "Wrong syntax: client_max_body_size" << std::endl;
	body_size = ft_stoi(string_value);
	this->_client_max_body_size = body_size;
}

void Location::setMaxBodySize(unsigned long value) {
	// std::cout << BLUE "Location setMaxBodySize called" RST << std::endl;
	this->_client_max_body_size = value;
}

/* for printing allowed methods*/
std::string Location::getPrintMethods() const {
	
	// std::cout << BLUE "Location getPrintMethods called" RST << std::endl;
	
	std::string res;
	if (_methods[4])
		res.insert(0, "HEAD");
	if (_methods[3])
	{
		if (!res.empty())
			res.insert(0, ", ");
		res.insert(0, "PUT");
	}
	if (_methods[2])
	{
		if (!res.empty())
			res.insert(0, ", ");
		res.insert(0, "DELETE");
	}
	if (_methods[1])
	{
		if (!res.empty())
			res.insert(0, ", ");
		res.insert(0, "POST");
	}
	if (_methods[0])
	{
		if (!res.empty())
			res.insert(0, ", ");
		res.insert(0, "GET");
	}
	return (res);
}