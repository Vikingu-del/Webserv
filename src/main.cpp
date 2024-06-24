/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 15:15:34 by ipetruni          #+#    #+#             */
/*   Updated: 2024/06/24 18:28:48 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "ServerSocket.hpp"

void sigpipeHandle(int sig) { if(sig) {}}

int main(int argc, char **argv) 
{
	// Logger::setState(OFF);
	if (argc == 1 || argc == 2) {
		try 
		{
			std::string			configPath;
			ConfigFileParser	parsing;
        	ServerSocket 		serverSocket;
			
			signal(SIGPIPE, sigpipeHandle);
			/* configuration file as argument or default configPath */
			configPath = (argc == 1 ? "configs/default.conf" : argv[1]);
			parsing.createCluster(configPath);
			// cluster.print(); // for checking
			serverSocket.setupServer(parsing.getServers());
			serverSocket.runServer();
		}
		catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			return (1);
		}
    }
    else 
	{
		Logger::logMsg(RED, CONSOLE_OUTPUT, "Error: wrong arguments");
		return (1);
	}
    return (0);
}
