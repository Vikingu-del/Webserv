/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfiguration.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 15:03:51 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/10 15:13:04 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ServerConfiguration.hpp"

// Constructors

ServerConfig::ServerConfig(){
	
}

// Destructors

ServerConfig::~ServerConfig(){
	
}

// Getters

const uint16_t &ServerConfig::getPort()
{
	return (this->_port);
}

const in_addr_t &ServerConfig::getHost()
{
	return (this->_host);
}

const std::string &ServerConfig::getServerName()
{
	return (this->_server_name);
}

const bool &ServerConfig::getAutoindex()
{
	return (this->_autoindex);
}