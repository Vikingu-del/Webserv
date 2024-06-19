/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:10:23 by kilchenk          #+#    #+#             */
/*   Updated: 2024/05/29 22:42:38 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Http.hpp"
#include <netinet/in.h>
#include "ConfigFileParser.hpp"
#include "ServerConfig.hpp"
#include <queue>
#include "defines.h"

class Client
{
	private:
		ServerConfig			_server;
		int						_clientSocket;
		struct sockaddr_in		_clientAddress;
		time_t					_lastMsg;
		std::string				_incompleteRequest;
		std::queue<std::string>	_requests;
		std::queue<std::string>	_responses;
		std::string 		   	_emptyResponse;
		
	public:
		/*ORTDOX*/
		Client();
		Client(const Client &copy);
		Client(ServerConfig & serv);
		Client  &operator=(const Client &copy);
		~Client();
		/*Geters*/
		const int                   &getSocket() const;
		const struct sockaddr_in    &getAddress() const;
		const time_t                &getLastTime() const;
		const ServerConfig          &getServer() const;
		const std::string			&getIncompleteRequest() const;
		/*Seters*/
		void						setSocket(int &socket);
		void						setAddress(sockaddr_in &address);
		void						setServer(ServerConfig &serv);
		void						setTime(); //update time
		void						setIncompleteRequest(const std::string &req);
		/*Methods*/
		void						addRequest(const std::string& req);
		bool						hasRequests();
		std::string					getNextRequest();
		void						addResponse(const std::string& resp);
		bool						hasResponses();
		std::string					&getCurrentResponse();
		void						removeCurrentResponse();
		void						clearClient();
};
