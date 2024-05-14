/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 16:11:16 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/14 18:23:52 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Location.hpp"

// !Constructors

Location::Location()
	:_path(""),
	_root(""),
	_autoindex(false),
	_index(""),
	_return(""),
	_alias(""),
	_client_max_body_size(MAX_CONTENT_LENGTH),
	_methods{1, 0, 0, 0, 0}
{
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
}

Location &Location::operator=(const Location &other) {
	if (this != &other) // !Check for self-assignment
	{
		_path = other._path;
		_root = other._root;
		_autoindex = other._autoindex;
		_index = other._index;
		_return = other._return;
		_alias = other._alias;
		_client_max_body_size = other._client_max_body_size;
		_methods = other._methods; // !Deep copy of the methods vector
	}
	return *this;
}


// !Destructor

Location::~Location() {
	// ? no specific cleanup needed
}


// !Getters

const std::string &Location::getPath() const {
	return(this->_path);
}

const std::string &Location::getRootLocation() const {
	return(this->_root);
}

const bool &Location::getAutoindex() const {
	return(this->_autoindex);
}

const std::string &Location::getIndexLocation() const {
	return(this->_index);
}

const std::vector<short> &Location::getMethods() const {
	return(this->_methods);
}

const std::string &Location::getReturn() const {
	return(this->_return);
}

const std::string &Location::getAlias() const {
	return(this->_alias);
}

const std::vector<std::string> &Location::getCgiPath() const
{
	return (this->_cgi_path);
}

const std::vector<std::string> &Location::getCgiExtension() const
{
	return (this->_cgi_ext);
}

const std::map<std::string, std::string> &Location::getExtensionPath() const {
	return(this->_ext_path);
}

const unsigned long &Location::getMaxBodySize() const {
	return(this->_client_max_body_size);
}

// !Setters

void Location::setPath(std::string value) {
	this->_path = value;
}

void Location::setRootLocation(std::string value) {
	if (ConfigFile::checkFileExistence(value) != 2)
		std::cerr << "Root of location" << std::endl; // TODO: Exception Class in ServerConfig
	this->_root = value;
}

void Location::setMethods(std::vector<std::string> methods) {

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
	if (value == "on" || value == "off")
		this->_autoindex = (value == "on");
	else
		std::cerr << "Wrong autoindex" << std::endl; // TODO: Exception Class in ServerConfig
}

void Location::setIndexLocation(std::string value) {
	this->_index = value;
}

void Location::setReturn(std::string value) {
	this->_return = value;
}

void Location::setAlias(std::string value) {
	this->_alias = value;
}

// TODO:

// Setters for CGIPATH and CGIEXTENTION

// TODO:

void Location::setMaxBodySize(std::string string_value) {
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
	this->_client_max_body_size = value;
}