/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 11:52:45 by eseferi           #+#    #+#             */
/*   Updated: 2024/05/16 18:41:23 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_FILE_HPP
#define CONFIG_FILE_HPP

#include "Webserv.hpp"

class ConfigFile {
    private:
        std::string _configFilePath;
        size_t      _numOfServers;
    public:
        ConfigFile();
        ConfigFile(std::string const path);
        ~ConfigFile();

        static int checkFileExistence(std::string const path);
        static int checkFilePermissons(std::string const path, int mode);
        static int checkFile(std::string const path, std::string const index);

        std::string getFileContent(std::string const path);
        std::string getPath();
        int getNumOfServers();

        
        void setNumOfServers(int num);
    public:
        class ConfigFileException : public std::exception {
            private:
                std::string _msg;
            public:
                ConfigFileException(std::string const &msg) : _msg(msg) {}
                virtual const char *what() const throw() {
                    return _msg.c_str();
                }
        };
};

#endif