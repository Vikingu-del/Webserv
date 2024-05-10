/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfiguration.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:17:52 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/10 18:04:56 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"

class ServerConfig {

	// !All needed atributes for server
	private:
		uint16_t						_port;
		in_addr_t						_host;
		std::string						_server_name;
		std::string						_root;
		unsigned long					_client_max_body_size;
		std::string						_index;
		bool							_autoindex;
		std::map<short, std::string>	_error_pages;
		std::vector<Location> 			_locations;
        struct sockaddr_in 				_server_address;
        int     						_listen_fd;

	// !Getters
	public:
		const uint16_t & getPort();
		const in_addr_t & getHost();
		const std::string & getServerName();
		const std::string & getRoot();
		const unsigned long & getClientMaxBody();
		const std::string & getIndex();
		const bool &  getAutoindex();
		const std::map<short, std::string> & getErrorPages();
		const std::vector<Location> & getLocations();
		const struct sockaddr_in & getServerArddres();
		const int & getListenFd();

	public:
		

	// !Constructors / Destructors
	public:
		ServerConfig();
		ServerConfig(const ServerConfig &other);
		ServerConfig &operator=(const ServerConfig &other);
		~ServerConfig();

	// !Methods
	public:
		void initErrorPages();
	
};
