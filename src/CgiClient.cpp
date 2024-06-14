// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   CgiClient.cpp                                      :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2024/06/04 16:55:16 by kilchenk          #+#    #+#             */
// /*   Updated: 2024/06/14 17:15:16 by eseferi          ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

// #include "CgiClient.hpp"
// #include <sys/fcntl.h>
// #include "utils.hpp"



// CgiClient::CgiClient(Client client, int epollFd)
// {
//     HTTP::Request req = HTTP::Request::deserialize(_response->getBody());
//     _client = client;
//     _isParsed = false;
//     _isRead = false;
//     _readBytes = 0;
//     _readTime = 0;
//     _codeStatus = 0;
//     _epollFd = epollFd;
//     // _cgiReq = //need input like .html or .php or .py
//     _response = &_client.getCurrentResponse(); //need to get response
//     _serverConfig = &_client.getServer();//need to get server config
//     _headerMap = &req.getHeaders();//need to get header from response
//     // _response = //need to set config
//     // _serverConfig = //need to set client
//     _cgiHandle = new CgiHandle(_serverConfig, _cgiReq, _epollFd);
//     _pid = _cgiHandle->getPid();
//     _body = &_response->getBody();//get body from response
//     // _requestBody =  //get body from server config
//     setPipe(*_cgiHandle);
//     toCgi(*_cgiHandle, *_requestBody);
// }

// CgiClient::~CgiClient()
// {
//     deleteChild(_cgiHandle->getPipeOut());
//     closePipe(*_cgiHandle);
//     close(_cgiHandle->getPipeOut());
//     kill(_pid, SIGKILL);
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

// void CgiClient::handleCgi()
// {
//     fromCgi(*_cgiHandle);
//     if (_codeStatus == 0)
//         return ;
//     else if(_cgiHandle->getExitStatus() == 500)
//         _codeStatus = 500;
//     //set status code to response
//     setContentLength();
// }

// void CgiClient::fromCgi(CgiHandle &cgi)
// {
//     char buffer[1024];
//     int in = read(cgi.getPipeOut(), buffer, sizeof(buffer));
//     if (in == -1 || in == 0)
//     {
//         _codeStatus = (_isRead) ? 200 : 500;
//     }
//     else
//     {
//         _body->append(buffer, in);
//         _readBytes += in;
//         if (_body->find("\r\n\r\n") != std::string::npos || _body->find("\n\n") != std::string::npos && !_isParsed)
//             handlePath(*_body);
//         _isRead = true;
//     }
//         // _response-> // need send error cpde to response
// }

// void CgiClient::parsePath()
// {
// 	std::vector<std::string> headerLines = split(_cgiPath, '\n');
// 	std::string key;
// 	std::string value;
// 	size_t pos;
// 	std::string header;
// 	for (size_t i = 0; i < headerLines.size(); i++)
// 	{
// 		header = headerLines[i];
// 		pos = header.find(":");
// 		if (pos != std::string::npos && pos <= header.length() - 2)
// 		{
// 			key = header.substr(0, header.find(":"));
// 			value = header.substr(header.find(":") + 2);
// 			(*_headerMap)[key] = value;
// 		}
// 		else if (header.find("HTTP/1.1") != std::string::npos)
// 		{
// 			pos = header.find(" ");
// 			if (pos != std::string::npos && pos <= header.length() - 2)
// 			{
// 				std::string status = header.substr(header.find(" ") + 1, 3);
// 				_codeStatus = atoi(status.c_str());
// 			}
// 		}
// 	}
// }


// void CgiClient::handlePath(std::string &body)
// {
// 	std::string::size_type pos;
// 	if (body.find("\r\n\r\n") != std::string::npos)
// 		pos = body.find("\r\n\r\n");
// 	else if (body.find("\r\n") != std::string::npos)
// 		pos = body.find("\r\n");
// 	if (pos != std::string::npos)
// 	{
// 		_cgiPath = body.substr(0, pos);
// 		if ((pos = body.find("\r\n\r\n\n")) != std::string::npos)
// 			body = body.substr(pos + 5);
// 		else if ((pos = body.find("\r\n\r\n")) != std::string::npos)
// 			body = body.substr(pos + 4);
// 		else if ((pos = body.find("\r\n\n")) != std::string::npos)
// 			body = body.substr(pos + 3);
// 		else if ((pos = body.find("\r\n")) != std::string::npos)
// 			body = body.substr(pos + 2);
//         _isParsed = true;
//         parsePath();
//     }
//     else
//     {
//         _cgiPath = _response->getBody();
//         //need to clear body
//     }
// }

// void CgiClient::setContentLength(void)
// {
//     std::stringstream ss;
//     ss << _readBytes;
//     // _response->setHeaders("Content-Length", ss.str()); !!
// }

// std::string &CgiClient::getResponseStr()
// {
//     // return _response->getBody();
// }

// int CgiClient::getCodeStatus()
// {
//     return _codeStatus;
// }

// int CgiClient::getPipe(void)
// {
//     return (_cgiHandle->getPipeOut());
// }

// void CgiClient::deleteChild(int childFd)
// {
// 	if (epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, childFd, NULL) == -1) {
// 		std::cerr << "Failed to remove client file descriptor from epoll instance." << std::endl;
// 	}
// }

// void CgiClient::closePipe(CgiHandle &cgi)
// {
// 	if (cgi.getPipeIn() != -1)
// 		close(cgi.getPipeIn());
// 	if (cgi.getPipeOut() != -1)
// 		close(cgi.getPipeOut());
// }