/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 15:15:34 by ipetruni          #+#    #+#             */
/*   Updated: 2024/06/05 14:09:43 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"
#include "Logger.hpp"

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
				Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "Error: wrong arguments");
			return (1);
		}
		parser.parseConfigFile(configFilePath);
		// parser.printServers();
		socket.setupServer(parser.getServers());
		socket.runServer();
	}
	catch (std::exception & ex) {
		Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "Exception: %s", ex.what());
		return 1;
	}
	return 0;
}
