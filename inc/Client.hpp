#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Webserv.hpp"
#include "Http.hpp"

class Client
{
    public:
        Client();
        Client(const Client &other);
        Client(ServerConfig &);
		    Client &operator=(const Client & rhs);
        ~Client();

        const int                 &getSocket() const;
        const struct sockaddr_in  &getAddress() const;
        const HTTP::Request       &getRequest() const;
        const time_t              &getLastTime() const;

        void                setSocket(int &);
        void                setAddress(sockaddr_in &);
        void                setServer(ServerConfig &);
        void                buildResponse();
        void                updateTime();

        void                clearClient();
        HTTP::Response      response;
        HTTP::Request       request;
        ServerConfig        server;
    private:
        int                 _client_socket;
        struct sockaddr_in  _client_address;
        time_t              _last_msg_time;
};


#endif // CLIENT_HPP
