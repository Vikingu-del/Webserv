#include "../../inc/configFile/ConfigFile.hpp"

// !Constructors 

ConfigFile::ConfigFile() : _configFilePath("configs/default.conf"), _numOfServers(0) {
	std::cout << GREEN BLD "ConfigFile default constructor called" RST << std::endl;
}

ConfigFile::ConfigFile(std::string const path) : _configFilePath(path), _numOfServers(0) {
	std::cout << GREEN BLD "ConfigFile PATH constructor called" RST << std::endl;
}

// !Destructor
ConfigFile::~ConfigFile() {
	std::cout << RED BLD "ConfigFile destructor called" RST << std::endl;
}

// !Getters
std::string ConfigFile::getPath() {
	std::cout << GREEN BLD "ConfigFile getPath called" RST << std::endl;
	return (this->_configFilePath); 
}

int ConfigFile::getNumOfServers() {
	std::cout << GREEN BLD "ConfigFile getNumOfServers called" RST << std::endl;
	return (this->_numOfServers); 
}

std::string ConfigFile::getFileContent(std::string const path) {
	std::cout << GREEN BLD "ConfigFile getFileContent called" RST << std::endl;
	if (path.empty() || path.length() == 0)
		throw std::invalid_argument("Path cannot be empty");
	
	std::ifstream config_file(path.c_str());
	if (!config_file || !config_file.is_open())
		throw std::runtime_error("Could not open file at path: " + path);
	
	std::stringstream stream_buffer;
	stream_buffer << config_file.rdbuf();
	return (stream_buffer.str());
}

// !Setters
void ConfigFile::setNumOfServers(int num) { 
	std::cout << GREEN BLD "ConfigFile setNumOfServers called" RST << std::endl;	
	this->_numOfServers = num;
}

// !Static methods
int ConfigFile::checkFileExistence(std::string const path) {
	std::cout << GREEN BLD "ConfigFile checkFileExistence called" RST << std::endl;
	struct stat buffer;
	if (stat(path.c_str(), &buffer) == 0) {
		if (buffer.st_mode & S_IFREG)
			return 1;
		else if (buffer.st_mode & S_IFDIR)
			return 2;
		else
			return 3;
	}
	return (-1);
}

int ConfigFile::checkFilePermissons(std::string const path, int mode) {
	std::cout << GREEN BLD "ConfigFile checkFilePermissons called" RST << std::endl;
	return (access(path.c_str(), mode));
}

int ConfigFile::checkFile(std::string const path, std::string const index) {
	std::cout << GREEN BLD "ConfigFile checkFile called" RST << std::endl;
	if (checkFileExistence(index) == 1 && checkFilePermissons(index, R_OK) == 0)
		return 0;
	if (!path.empty() && checkFileExistence(path + index) == 1 && checkFilePermissons(path + index, R_OK) == 0)
		return 0;
	return -1;
}