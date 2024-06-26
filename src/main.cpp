/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 15:15:34 by ipetruni          #+#    #+#             */
/*   Updated: 2024/06/26 01:57:01 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

void sigpipeHandle(int sig) { if(sig) {}} // to avoid crash on SIGPIPE

int main(int argc, char **argv)
{

	std::string configFilePath;
	ConfigFileParser parser;
	ServerSocket manager;
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
		signal(SIGPIPE, sigpipeHandle);
		parser.parseConfigFile(configFilePath);
		// parser.printServers();
		manager.setupServers(parser.getServers());
		manager.runServers();
	}
	catch (std::exception & ex) {
		Logger::logMsg("\x1B[31m", CONSOLE_OUTPUT, "Exception: %s", ex.what());
		return 1;
	}
	return 0;
}

