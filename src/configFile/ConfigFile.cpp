/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 17:07:47 by ipetruni          #+#    #+#             */
/*   Updated: 2024/06/25 12:48:05 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

//! Checks if a file or directory exists at the given path.
int ConfigFile::checkFileExistence(std::string const path) {
	// std::cout << GREEN BLD "ConfigFile checkFileExistence called" RST << std::endl;
	struct stat buffer;
	if (stat(path.c_str(), &buffer) == 0) {
		if (buffer.st_mode & S_IFREG)
			return THIS_IS_FILE;
		else if (buffer.st_mode & S_IFDIR)
			return THIS_IS_DIR;
		else if (buffer.st_mode & S_IFLNK)
			return THIS_IS_LINK;
		else
			return THIS_FILE_DOESNT_EXIST;
	}
	return THIS_FILE_DOESNT_EXIST;
}

//! Check if the file has specified permission
int ConfigFile::checkFilePermissons(std::string const path, int mode) {
	return (access(path.c_str(), mode));
}

//! Get the content of the file at the given path
std::string ConfigFile::getFileContent(std::string const path) {
	if (path.empty() || path.length() == 0)
		throw ConfigFileException("Path cannot be empty");
	std::ifstream config_file(path.c_str());
	if (!config_file || !config_file.is_open())
		throw  ConfigFileException("Could not open file at path: " + path);
	std::stringstream stream_buffer;
	stream_buffer << config_file.rdbuf();
	return (stream_buffer.str());
}


// !Getters
std::string ConfigFile::getPath() {
	// std::cout << GREEN BLD "ConfigFile getPath called" RST << std::endl;
	return (this->_configFilePath); 
}

int ConfigFile::getNumOfServers() {
	// std::cout << GREEN BLD "ConfigFile getNumOfServers called" RST << std::endl;
	return (this->_numOfServers); 
}


// !Setters
void ConfigFile::setNumOfServers(int num) { 
	// std::cout << GREEN BLD "ConfigFile setNumOfServers called" RST << std::endl;	
	this->_numOfServers = num;
}


//! Checks if the file exists and has the required permissions
int ConfigFile::checkFile(std::string const path, std::string const index) {
    // If no specific permissions are required, only check if the file exists
    if (index == NO_PERMISSIONS) {
        return checkFileExistence(path);
    }

    // Otherwise, check if the file exists and has the required permissions
    if (checkFileExistence(index) == 1 && checkFilePermissons(index, R_OK) == 0)
        return 0;
    if (!path.empty() && checkFileExistence(path + index) == 1 && checkFilePermissons(path + index, R_OK) == 0)
        return 0;
    return -1;
}

//! Constructors 

ConfigFile::ConfigFile() : _configFilePath("configs/default.conf"), _numOfServers(0) {
	// std::cout << GREEN BLD "ConfigFile default constructor called" RST << std::endl;
}

ConfigFile::ConfigFile(std::string const path) : _configFilePath(path), _numOfServers(0) {
	// std::cout << GREEN BLD "ConfigFile PATH constructor called" RST << std::endl;
}

//! Destructor
ConfigFile::~ConfigFile() {
	std::cout << RED BLD "ConfigFile destructor called" RST << std::endl;
}