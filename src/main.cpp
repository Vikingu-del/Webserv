/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 15:15:34 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/10 11:38:10 by eseferi          ###   ########.fr       */
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
			case 2:
				configFilePath = argv[1];
				parser.parseConfigFile(configFilePath);
			default:
				throw std::invalid_argument("Wrong number arguments");
				return 1;
		}
	}
	catch (std::exception & ex) {
		std::cerr << ex.what() << std::endl;
		return 1;
	}
	
	
	return 0;
}


// int main(int argc, char **argv) 
// {
//     std::string config;
//     ConfigParser cluster;
//     ServerManager master;

//     try 
//     {
//         signal(SIGPIPE, sigpipeHandle);

//         switch (argc)
//         {
//         case 1:
//             config = "configs/default.conf";
//             cluster.createCluster(config);
//             master.setupServers(cluster.getServers());
//             master.runServers();
//             break;
//         case 2:
//             config = argv[1];
//             cluster.createCluster(config);
//             master.setupServers(cluster.getServers());
//             master.runServers();
//             break;
//         default:
//             Logger::logMsg(RED, CONSOLE_OUTPUT, "Error: wrong arguments");
//             return 1;
//         }
//     }
//     catch (std::exception &e) 
//     {
//         std::cerr << e.what() << std::endl;
//         return 1;
//     }

//     return 0;
// }