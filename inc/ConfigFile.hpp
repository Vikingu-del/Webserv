/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 11:52:45 by eseferi           #+#    #+#             */
/*   Updated: 2024/05/10 12:50:46 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

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
};

