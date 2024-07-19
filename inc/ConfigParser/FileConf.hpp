#ifndef FILECONF_HPP
#define FILECONF_HPP

#include "Webserv.hpp"

class FileConf {
	private:
		std::string		_path;
		size_t			_size;	// проверить нужен ли

	public:
		FileConf();
		FileConf(std::string const path);
		~FileConf();

		static int getTypePath(std::string const path);
		static int checkFile(std::string const path, int mode);
		static int isFileExistAndReadable(std::string const path, std::string const index);

		std::string	readFile(std::string path);
		std::string getPath();
		int getSize();
};

#endif