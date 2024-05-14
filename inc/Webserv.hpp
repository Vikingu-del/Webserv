/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 11:21:20 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/14 18:04:35 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Default libs
#include <iostream>


// STL libs
#include <map>
#include <vector>

// Network / IP etc.
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/select.h>
# include <arpa/inet.h>

// Webserv libs
#include <fstream>
#include <sstream>
#include <sys/stat.h>
// #include <types.h>
#include <unistd.h>

#include "defines.h"
#include "ConfigFileParser.hpp"
#include "ServerConfiguration.hpp"
#include "ConfigFile.hpp"
#include "Location.hpp"
#include "Client.hpp"
#include "WebservUtils.hpp"
/* Webserv.hpp */