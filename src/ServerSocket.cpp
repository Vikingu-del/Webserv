/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kilchenk <kilchenk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:02:11 by kilchenk          #+#    #+#             */
/*   Updated: 2024/05/10 16:22:44 by kilchenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ServerSocket.hpp"

ServerSocket::ServerSocket()
{

}

ServerSocket::~ServerSocket()
{
    
}

void ServerSocket::setupServer(std::vector<ServerConfig> serv)
{
    _servers = serv;
    char buf = INET_ADDRSTRLEN;
    bool serverDup;
    
}