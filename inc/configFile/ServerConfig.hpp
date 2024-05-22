/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:17:52 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/22 18:43:36 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONFIGURATION_HPP
#define SERVER_CONFIGURATION_HPP

#include "Location.hpp"
#include <cstring>
#include <arpa/inet.h>
#include <cstdlib> 

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
		std::vector<Location> 			_locations;
		int     						_listen_fd;
		struct sockaddr_in 				_server_address;
		std::map<short, std::string>	_error_pages;

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


	// !Setters
	public:
		void setServerName(std::string server_name);
		void setHost(std::string parametr);
		void setRoot(std::string root);
		void setFd(int fd);
		void setPort(std::string parametr);
		void setClientMaxBodySize(std::string parametr);
		void setErrorPages(const std::vector<std::string> &parametr);
		void setIndex(std::string index);
		void setLocation(std::string nameLocation, std::vector<std::string> parametr);
		void setAutoindex(std::string autoindex);


	// !Constructors / Destructors
	public:
		ServerConfig();
		ServerConfig(const ServerConfig &other);
		ServerConfig &operator=(const ServerConfig &other);
		~ServerConfig();

	// !Methods
	public:
		void initErrorPages();
		void checkToken(std::string & parametr);
		int	isValidLocation(Location & location) const;
		bool isValidHost(std::string host) const;
		void bindServer();
		bool checkLocaitons() const;
		bool isValidErrorPages();
	// !Exceptions
	public:
		class ServerConfigException : public std::exception
		{
			private:
				std::string _message;
			public:
				ServerConfigException(std::string message) throw()
				{
					_message = RED "Server Config Error: " RST + message;
				}
				virtual const char* what() const throw()
				{
					return (_message.c_str());
				}
				virtual ~ServerConfigException() throw() {}
		};
};

#endif