/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfiguration.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:17:52 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/10 15:15:16 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"

class ServerConfig {

	private:
		uint16_t						_port;
		in_addr_t						_host;
		std::string						_server_name;
		std::string						_root;
		unsigned long					_client_max_body_size;
		std::string						_index;
		bool							_autoindex;
		std::map<short, std::string>	_error_pages;
		// std::vector<Location> 			_locations;
        struct sockaddr_in 				_server_address;
        int     						_listen_fd;

	// Getters for private atributes
	public:
		const uint16_t & getPort();
		const in_addr_t & getHost();
		const std::string & getServerName();
		const std::string & getRoot();
		const bool &ServerConfig::getAutoindex();
		const std::string &ServerConfig::getRoot();

	public:
		ServerConfig();
		~ServerConfig();

};
