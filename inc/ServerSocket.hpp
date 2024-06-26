#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

class CgiHandler;
class Client;

class ServerSocket {
private:
    int epoll_fd;
    std::vector<ServerConfig> _servers;
    std::map<int, Client> _clientsMap;
    std::map<int, ServerConfig> _serversMap;
    std::map<int, CgiHandler> _cgiPipeMap;

public:
    ServerSocket();
    ~ServerSocket();

    void addToEpoll(const int fd, uint32_t events);
    void modifyEpoll(int fd, uint32_t events);
    void removeFromEpoll(int fd);
    void acceptNewConnection(ServerConfig &serverConfig);
    void listenServers();
    void assignServer(Client &);
    void handleReqBody(Client &);
    void readRequest(const int& fd, Client &);
    void readCgiResponse(Client &, CgiHandler &);
    void sendCgiBody(Client &, CgiHandler &);
    void sendResponse(const int &i, Client &);
    void handleEpollIn(int fd);
    void handleEpollOut(int fd);
    // void parseRequest(Client *client, const std::string &data);
    // void handleCgiEvent(int fd, uint32_t events);
    // void handleCgiRequest(Client *client);
    // void sendResponse(const int fd, Client *client);
    void closeConnection(int clientFd);
    void runServers();
    // void handleEpollIn(int fd);
    // void handleEpollOut(int fd);
    void setupServers(std::vector<ServerConfig> serv);
    void checkTimeout();
    int getEpollFd() const;
    std::map<int, CgiHandler>& getCgiPipeMap() { return _cgiPipeMap; }
    void removeFdFromMonitor(int fd);
};

#endif // SERVER_SOCKET_HPP
