/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 11:53:16 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/09 14:19:22 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ConfigFileParser.hpp"

// Constructors
ConfigFileParser::ConfigFileParser() {
	_numOfServers = 0;
}

// Destructor
ConfigFileParser::~ConfigFileParser() {
}

// Methods
int ConfigFileParser::parseConfigFile(std::string string) {
	std::cout << string << std::endl;
	return (0);	
}
