#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

#include "Webserv.hpp"
#include "Client.hpp"
#include "Response.hpp"

/**
 * ServerSocket
 * * operates the webserver and is responsible for
 * - runing servers with configration extracted from config file
 * - establishing new connections with clients and receive/send requests/responses.
 */
class ServerSocket
{
    public:                 
        ServerSocket();
        ~ServerSocket();
        void    setupServers(std::vector<ServerConf>);
        void    runServers();
        
    private:
        std::vector<ServerConf> _servers;
        std::map<int, ServerConf> _servers_map;
        std::map<int, Client> _clients_map;
        fd_set     _recv_fd_pool;
        fd_set     _write_fd_pool;
        int        _biggest_fd;

        void acceptNewConnection(ServerConf &);
        void checkTimeout();
        void initializeSets();
        void readRequest(const int &, Client &);
        void handleReqBody(Client &);
        void sendResponse(const int &, Client &);
        void sendCgiBody(Client &, Cgi &);
        void readCgiResponse(Client &, Cgi &);
        void closeConnection(const int);
        void assignServer(Client &);
        void addToSet(const int , fd_set &);
        void removeFromSet(const int , fd_set &);
};


#endif // ServerSocket_HPP