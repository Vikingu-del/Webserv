/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandle.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kilchenk <kilchenk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 16:55:20 by kilchenk          #+#    #+#             */
/*   Updated: 2024/06/05 15:03:45 by kilchenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandle.hpp"
#include "utils.hpp"

CgiHandle::CgiHandle(ServerConfig *config, std::string extansion, int epollFd, Location *locationConfig)
{
    _config = config;
    _cgiReq = extansion;
    _epollFd = epollFd;
    _locationConfig = locationConfig;
    _pathStr = "";
    _exitStatus = 0;
    _length = 0;
    _pid = -1;
    _pipeIn[0] = 0;
    _pipeIn[1] = 0;
    _pipeOut[0] = 0;
    _pipeOut[1] = 0;
    _argv = NULL;
    _envp = NULL;
    _path = NULL;
    
}

CgiHandle::~CgiHandle()
{
    if (_path)
    {
        free(_path);
        delete _path;
    }
    if (_argv)
    {
        free(_argv);
        delete _argv;
    }
    if (_envp)
    {
        free(_envp);
        delete _envp;
    }
}

CgiHandle::CgiHandle(const CgiHandle &copy)
{
    *this = copy;
}

CgiHandle &CgiHandle::operator=(const CgiHandle &copy)
{
    if (this == &copy)
        return *this;
    _config = copy._config;
    _cgiReq = copy._cgiReq;
    _epollFd = copy._epollFd;
    _locationConfig = copy._locationConfig;
    _pathStr = copy._pathStr;
    _exitStatus = copy._exitStatus;
    _length = copy._length;
    _pid = copy._pid;
    _pipeIn[0] = copy._pipeIn[0];
    _pipeIn[1] = copy._pipeIn[1];
    _pipeOut[0] = copy._pipeOut[0];
    _pipeOut[1] = copy._pipeOut[1];
    _argv = copy._argv;
    _envp = copy._envp;
    _path = copy._path;
    return *this;
}

void CgiHandle::initEnv()
{
    std::stringstream ss;
    ss << _config->getMimeType("content-length");
    std::string content_length = ss.str();
    content_length = utils::strToInt(content_length);
    if (content_length != 0)
        _env["CONTENT_LENGTH"] = content_length;
    else
        _env["CONTENT_LENGTH"] = "0";
    ss.clear();
    if (_config->getMimeType("content-type").size() > 0)
        _env["CONTENT_TYPE"] = _config->getMimeType("content-type");
    // _env["QUERY_STRING"] = _config->getQuery();
    // _env["REQUEST_URI"] = _config->getUri() + _config->getUriSuffix();
    _env["REDIRECT_STATUS"] = "200";
    // _env["SCRIPT_NAME"] = _config->getUri();
    // _env["PATH_INFO"] = "/" + _config->getUriSuffix();
    _env["PATH_TRANSLATED"] = _config->getRoot() + _env["PATH_INFO"];
    // _env["SCRIPT_FILENAME"] = _config->getRoot() + _config->getUri();
    _env["DOCUMENT_ROOT"] = _config->getRoot();
    _env["REQUEST_METHOD"] = _locationConfig->getMethods();
    // _env["SERVER_PROTOCOL"] = _config->getProtocol();
    _env["SERVER_SOFTWARE"] = "AMANIX";
    _env["GATEWAY_INTERFACE"] = "CGI/1.1";
    _env["REQUEST_SCHEME"] = "http";
    ss << _config->getPort();
    std::string port = ss.str();
    _env["SERVER_ADDR"] = _config->getHost();
    _env["SERVER_PORT"] = port;
    _env["SERVER_NAME"] = "localhost";
    _env["REMOTE_ADDR"] = _config->getHost();
    _env["REMOTE_PORT"] = port;
    _env["HTTP_HOST"] = _config->getHost() + ":" + port;
    _env["HTTP_CONNECTION"] = _config->getMimeType("connection");
    _env["HTTP_UPGRADE_INSECURE_REQUESTS"] = _config->getMimeType("upgrade-insecure-requests");
    _env["HTTP_USER_AGENT"] = _config->getMimeType("user-agent");
    _env["HTTP_ACCEPT"] = _config->getMimeType("accept");
    _env["HTTP_ACCEPT_ENCODING"] = _config->getMimeType("accept-encoding");
    _env["HTTP_ACCEPT_LANGUAGE"] = _config->getMimeType("accept-language");
    // _env["HTTP_REFERER"] = "http://" + _config->getHost() + ":" + port + "/cgi-bin" + _config->getUriSuffix();
    _env["HTTP_COOKIE"] = _config->getMimeType("cookie");
    ss.clear();
}