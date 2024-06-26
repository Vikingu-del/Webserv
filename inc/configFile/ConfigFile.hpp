/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 11:52:45 by eseferi           #+#    #+#             */
/*   Updated: 2024/06/25 12:43:02 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_FILE_HPP
#define CONFIG_FILE_HPP

#include <string>

class ConfigFile {
    
    //! Private attributes
    private:
        std::string _configFilePath;
        size_t      _numOfServers;
    //! Constructors / Destructor
    public:
        ConfigFile();
        ConfigFile(std::string const path);
        ~ConfigFile();
    //! Methods
    public:
        //! Checks if a file or directory exists at the given path.
        static int checkFileExistence(std::string const path);
        static int checkFilePermissons(std::string const path, int mode);
        static int checkFile(std::string const path, std::string const index);
        
        //! Get the content of the file at the given path
        std::string getFileContent(std::string const path);
        std::string getPath();
        
        //! Getters
        int getNumOfServers();
        void setNumOfServers(int num);
    //! Exceptions 
    public:
        class ConfigFileException : public std::exception {
            private:
                std::string _msg;
            public:
                ConfigFileException(std::string const &msg) : _msg(msg) {}
                virtual ~ConfigFileException() throw() {}
                virtual const char *what() const throw() {
                    return _msg.c_str();
                }
        };
};

#endif