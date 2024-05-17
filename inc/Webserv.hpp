/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 11:21:20 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/16 19:38:16 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define CYAN "\033[0;36m"
#define PINK "\033[0;35m"
#define BLD "\033[1m"
#define RST "\033[0m"


//! Default libs
#include <iostream>

//! STL libs
#include <map>
#include <vector>

//! Network / IP etc.
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/select.h>
# include <arpa/inet.h>
# include <iterator>

//! Webserv libs
#include <fstream>
#include <sstream>
#include <sstream>
#include <sys/stat.h>
// #include <types.h>
#include <unistd.h>
#include <cstddef>
#include <cstring>

#include "defines.h"

//! ConfigFileParsingUtils
#include "configFile/ConfigFile.hpp"
#include "configFile/ConfigFileParser.hpp"
#include "configFile/ConfigFileUtils.hpp"
#include "configFile/Location.hpp"
#include "configFile/ServerConfiguration.hpp"



#include "Http.hpp"
#include "Client.hpp"

// #include "HttpResponse.hpp"
#include "utils.hpp"

/* Webserv.hpp */
#endif