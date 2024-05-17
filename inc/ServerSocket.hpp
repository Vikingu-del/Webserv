/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:03:11 by kilchenk          #+#    #+#             */
/*   Updated: 2024/05/16 19:32:17 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_SOCKET_HPP
# define SERVER_SOCKET_HPP
# include "Webserv.hpp"

class ServerSocket
{
    private:
        std::vector<ServerConfig> _servers;
        std::map<int, ServerConfig> _servers_map;
        std::map<int, Client> _clients_map;
        fd_set     _read_fd;
        fd_set     _write_fd;
        int        _biggest_fd;

        void acceptNewConnection(ServerConfig &serv);
        void checkTimeout();
        void listenServer();
        void readRequest(const int &fd, Client &client);
        void handleReqBody(Client &client);
        void sendResponse(const int &fd, Client &client);
        // void sendCgiBody(Client &, CgiHandler &);
        // void readCgiResponse(Client &, CgiHandler &);
        void closeConnection(const int fd);
        void assignServer(Client &client);
        void addToSet(const int fd, fd_set &set);
        void removeFromSet(const int fd, fd_set &set);
    public:
        ServerSocket();
        ~ServerSocket();
        void    setupServer(std::vector<ServerConfig> serv);
        void    runServer();
};

#endif
