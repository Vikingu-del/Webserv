/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kilchenk <kilchenk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 11:21:20 by ipetruni          #+#    #+#             */
<<<<<<< .merge_file_fS6R8H
/*   Updated: 2024/05/10 16:05:29 by kilchenk         ###   ########.fr       */
=======
/*   Updated: 2024/05/10 17:46:51 by ipetruni         ###   ########.fr       */
>>>>>>> .merge_file_id48Q6
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
<<<<<<< .merge_file_fS6R8H
#include "Client.hpp"
=======
#include "Location.hpp"
#include "WebservUtils.hpp"

>>>>>>> .merge_file_id48Q6
/* Webserv.hpp */