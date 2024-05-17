/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:10:23 by kilchenk          #+#    #+#             */
/*   Updated: 2024/05/16 19:51:52 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Http.hpp"
#include <netinet/in.h>
#include "ServerConfiguration.hpp"


class Client
{
    private:
        struct sockaddr_in  _client_address;
        time_t              _last_msg;
        int                 _client_socket;
        HTTP::Request       _request;
        HTTP::Response      _response;
        
    public:
        /*ORTDOX*/
        Client();
        Client(const Client &copy);
        Client  &operator=(const Client &copy);
        Client(ServerConfig & server);
        ~Client();
        /*Geters*/
        const int                   &getSocket() const;
        const struct sockaddr_in    &getAddress() const;
        const time_t                &getLastTime() const;
        const HTTP::Request         &getRequest() const;
        /*Seters*/
        void                        setSocket(int &socket);
        void                        setAddress(sockaddr_in &address);
        void                        setServer(ServerConfig &serv);
        void                        setTime(); //update time
        /*Another*/
        void                        clearClient();
        ServerConfig                server;
        
};
