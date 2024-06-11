/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiClient.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kilchenk <kilchenk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 12:53:14 by kilchenk          #+#    #+#             */
/*   Updated: 2024/06/11 17:02:11 by kilchenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_CLIENT_HPP
# define CGI_CLIENT_HPP

#include "CgiHandle.hpp"
#include "Webserv.hpp"

class Client;
class RequestHandler;
class ServerConfig;
class HTTP::Response;
class Location;
class CgiHandle;

class CgiClient
{
    private:
        std::map<std::string, std::string> *_headerMap;
        Client      _client;
        HTTP::Response *_response;
        CgiHandle   *_cgiHandle;
        ServerConfig *_serverConfig;
        // Location    _location;
        std::string _cgiPath; //HEADER
        std::string _cgiReq; //cgi extansion
        std::string *_body;
        std::string *_requestBody;
        pid_t       _pid;
        int         _readBytes;
        int         _readTime;
        int         _codeStatus;
        int         _epollFd;
        bool        _isRead;
        bool        _isParsed;
    public:
    //ortodox
    CgiClient(Client client, int epollFd);
    ~CgiClient();
    CgiClient(const CgiClient &copy);
    CgiClient &operator=(const CgiClient &copy);
    //getters
    // HTTP    &getResponse();
    std::string &getResponseStr();
    int         getCodeStatus();
    int         getPipe(void);
    //setters
    void    setContentLength(void);
    void    setPipe(CgiHandle &cgi);
    //other
    void    handleCgi();
    void    fromCgi(CgiHandle &cgi);
    void    toCgi(CgiHandle &cgi, std::string &body);
    void    handlePath(std::string &body);
    void    parsePath();
    void    closePipe(CgiHandle &cgi);
    void    deleteChild(int childFd);
};

#endif
