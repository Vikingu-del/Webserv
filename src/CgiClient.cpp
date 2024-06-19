/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiClient.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kilchenk <kilchenk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 16:55:16 by kilchenk          #+#    #+#             */
/*   Updated: 2024/06/11 17:18:10 by kilchenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "CgiClient.hpp"
// #include <sys/fcntl.h>


// CgiClient::CgiClient(Client client, int epollFd)
// {
//     _client = client;
//     _isParsed = false;
//     _isRead = false;
//     _readBytes = 0;
//     _readTime = 0;
//     _codeStatus = 0;
//     _epollFd = epollFd;
//     // _cgiReq = //need input like .html or .php or .py
//     // _response = //need to get response
//     // _serverConfig = //need to get server config
//     // _headerMap = //need to get header from response
//     // _response = //need to set config
//     // _serverConfig = //need to set client
//     // _cgiHandle = new CgiHandle(_serverConfig, _cgiReq, _epollFd, _client.getRequest());
//     // _pid = _cgiHandle->getPid();
//     // _body = //get body from response
//     //_requestBody =; //get body from server config
//     setPipe(*_cgiHandle);
//     toCgi(*_cgiHandle, *_requestBody);
// }

// CgiClient::~CgiClient()
// {
//     delete _cgiHandle;
// }

// void CgiClient::setPipe(CgiHandle &cgi)
// {
//     int flag;
//     if ((flag = fcntl(cgi.getPipeOut(), F_GETFL, 0)) == -1 || fcntl(cgi.getPipeOut(), F_SETFL, flag | O_NONBLOCK) == -1)
//     {
//         std::cerr << "Error: fcntl failed" << std::endl;
//         _codeStatus = 500;
//         return ;
//     }
// }

// void CgiClient::toCgi(CgiHandle &cgi, std::string &body)
// {
    
//     if (cgi.getLength() > 0)
//     {
//         int out = write(cgi.getPipeIn(), body.c_str(), body.length());
//         if (out == -1)
//         {
//             std::cerr << "Error: write failed" << std::endl;
//             _codeStatus = 500;
//             // _response->// need send error cpde to response
//         } 
//         else
//         {
//             body = body.substr(out);
//             cgi.erasseLength(out);
//             if (cgi.getLength() == 0)
//                 close(cgi.getPipeIn());
//         }
//     }
// }

