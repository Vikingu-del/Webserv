/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 15:15:34 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/09 14:39:15 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Webserv.hpp"

int main(int argc, char **argv)
{

	std::string configFilePath;
	ConfigFileParser parser;

	try {
		switch (argc) 
		{
			case 1:
				configFilePath = "configs/default.conf";
				parser.parseConfigFile(configFilePath);
				// After parsing setup all needed info
				// Run the servers
			case 2:
				configFilePath = argv[1];
				parser.parseConfigFile(configFilePath);
				// After parsing setup all needed info
				// Run the servers
			default:
				throw std::invalid_argument("Wrong arguments");
				return 1;
		}
	}
	catch (std::exception & ex) {
		std::cerr << ex.what() << std::endl;
		return 1;
	}
	return 0;
}
