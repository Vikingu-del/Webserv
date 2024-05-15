/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:10:23 by kilchenk          #+#    #+#             */
/*   Updated: 2024/05/15 12:31:15 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP
#include "Webserv.hpp"

class Client
{
    private:
        struct sockaddr_in  _client_address;
        time_t              _last_msg;
        int                 _client_socket;
        
    public:
        /*ORTDOX*/
        Client();
        Client(const Client &copy);
        Client  &operator=(const Client &copy);
        Client(ServerConfig &server);
        ~Client();
        /*Geters*/ //need request class
        const int                   &getSocket() const;
        const struct sockaddr_in    &getAddress() const;
        const time_t                &getLastTime() const;
        /*Seters*/ //need request class
        void                        setSocket(int &socket);
        void                        setAddress(sockaddr_in &address);
        void                        setServer(ServerConfig &serv);
        void                        setTime(); //update
        /*Another*/ //need request and response classes
        // void                        clearClient();// need request and response
        ServerConfig                server;
};


#endif