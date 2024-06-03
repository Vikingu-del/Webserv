/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 15:15:34 by ipetruni          #+#    #+#             */
/*   Updated: 2024/06/01 14:49:38 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"

int main(int argc, char **argv)
{

	std::string configFilePath;
	ConfigFileParser parser;
	ServerSocket socket;
	try {
		switch (argc) 
		{
			case 1:
				configFilePath = "configs/siege.conf";
				break;
			case 2:
				configFilePath = argv[1];
				break;
			default:
				throw std::invalid_argument("Wrong number arguments");
				return 1;
		}
		parser.parseConfigFile(configFilePath);
		// parser.printServers();
		socket.setupServer(parser.getServers());
		socket.runServer();
	}
	catch (std::exception & ex) {
		std::cerr << ex.what() << std::endl;
		return 1;
	}
	return 0;
}
