/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 11:21:20 by ipetruni          #+#    #+#             */
/*   Updated: 2024/06/03 17:00:26 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP


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
#include <unistd.h>
#include <cstddef>
#include <cstring>

#include "defines.h"

//! Config File Parsing
#include "configFile/ConfigFile.hpp"
#include "configFile/ConfigFileParser.hpp"
#include "configFile/Location.hpp"
#include "ServerConfig.hpp"

//! Logger
#include "Logger.hpp"



#include "Client.hpp"
// #include "Http.hpp"

// #include "HttpResponse.hpp"
#include "utils.hpp"

/* Webserv.hpp */
#endif