#include "ConfigFile.hpp"

// Constructors / Destructor
ConfigFile::ConfigFile() : _configFilePath("configs/default.conf"), _numOfServers(0) {}
ConfigFile::ConfigFile(std::string const path) : _configFilePath(path), _numOfServers(0) {}
ConfigFile::~ConfigFile() {}

// Getters
std::string ConfigFile::getPath() { return this->_configFilePath; }
int ConfigFile::getNumOfServers() { return this->_numOfServers; }
std::string ConfigFile::getFileContent(std::string const path) {
if (path.empty() || path.length() == 0)
        throw std::invalid_argument("Path cannot be empty");
    std::ifstream config_file(path.c_str());
    if (!config_file || !config_file.is_open())
        throw std::runtime_error("Could not open file at path: " + path);
    std::stringstream stream_buffer;
    stream_buffer << config_file.rdbuf();
    return stream_buffer.str();
}

// Setters
void ConfigFile::setNumOfServers(int num) { this->_numOfServers = num; }

// Static methods
int ConfigFile::checkFileExistence(std::string const path) {
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
    return (access(path.c_str(), mode));
}

int ConfigFile::checkFile(std::string const path, std::string const index) {
    if (checkFileExistence(index) == 1 && checkFilePermissons(index, R_OK) == 0)
        return 0;
    if (!path.empty() && checkFileExistence(path + index) == 1 && checkFilePermissons(path + index, R_OK) == 0)
        return 0;
    return -1;
}