/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 11:52:26 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/09 12:26:34 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGFILEPARSER_HPP
#define CONFIGFILEPARSER_HPP

#include "Webserv.hpp"

class ConfigFileParser
{
	// Private attributes
	private:
		std::vector<std::string>	_serversConfig;
		size_t						_numOfServers;
		// std::vector<ServerConfig> _servers;
	
	// Constructor / Destructor
	public:
		ConfigFileParser();
		~ConfigFileParser();
	
	// Methods
	public:
		int parseConfigFile(std::string string);
};

#endif