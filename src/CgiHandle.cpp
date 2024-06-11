/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandle.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kilchenk <kilchenk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 16:55:20 by kilchenk          #+#    #+#             */
/*   Updated: 2024/06/11 17:47:30 by kilchenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandle.hpp"
#include "utils.hpp"

CgiHandle::CgiHandle(ServerConfig *config, std::string extansion, int epollFd, HTTP::Request &req)
{
    _config = config;
    _cgiReq = extansion;
    _epollFd = epollFd;
    _req = req;
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
    initEnv();
    execCgi();
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
    closePipe();
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
    _req = copy._req;
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
    std::stringstream contentLength;
    std::stringstream ss;
    ss << _config->getMimeType("content-length");
    std::string content_length = ss.str();
    contentLength << utils::strToInt(content_length);
    content_length = contentLength.str();
    if (!content_length.empty())
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
    _env["REQUEST_METHOD"] = _req.getMethod();
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

void CgiHandle::execCgi()
{
    setPath();
    setArgv();
    createArray();
        return ;
    _pid = fork();
    if (_pid == -1)
    {
        std::cerr << "Error: fork failed" << std::endl;
        _exitStatus = 5;
        closePipe();
        return ;
    }
    else if (_pid == 0)
    {
        if (dup2(_pipeIn[0], 0) == -1 || dup2(_pipeOut[1], 1) == -1)
        {
            std::cerr << "Error: dup2 failed" << std::endl;
            _exitStatus = 5;
            closePipe();
            exit(1);
        }
        if (execve(getPath().c_str(), _argv, _envp) == -1)
        {
            std::cerr << "Error: execve failed" << std::endl;
            _exitStatus = 5;
            closePipe();
            exit(1);
        }
        closePipe();
    }
}

void CgiHandle::setPath()
{
    std::string path(getenv("PWD"));
    path += "/" + _config->getRoot(); //+ _config->getUri(); // need uri
    _path = strdup(path.c_str());
}

void CgiHandle::setArgv()
{
    _argv = new char*[3];
    _argv[0] = strdup(getPath().c_str());
    _argv[1] = strdup(_path);
    _argv[2] = NULL;
}

std::string CgiHandle::getPath()
{
    if (checkAccepting().size() > 0)
        return checkAccepting();
    if (_cgiReq == ".py")
        return "/usr/bin/python3";
    else if (_cgiReq == ".sh")
        return "/bin/sh";
    else if (_cgiReq == ".php")
        return "/usr/bin/php";
    else
        return "";
}

std::string CgiHandle::checkAccepting()
{
    std::ifstream file(_path);
    if (!file.is_open())
        return "";
    std::string line;
    std::getline(file, line);
    if (line.find("#!") == std::string::npos)
        return "";
    else
    {
        std::stringstream ss(line);
        return(line.substr(line.find("#!") + 2));
    }
}

void CgiHandle::createArray()
{
    std::map<std::string, std::string>::iterator it;
    int i = 0;
    _envp = new char*[_env.size() + 1];
    for (it = _env.begin(); it != _env.end(); it++)
    {
        _envp[i] = strdup((it->first + "=" + it->second).c_str());
        i++;
    }
    _envp[i] = NULL;
}

int CgiHandle::setPipe()
{
    if (pipe(_pipeIn) == -1 || pipe(_pipeOut) == -1)
    {
        std::cerr << "Error: pipe failed" << std::endl;
        return -1;
    }
    return 0;
}

int CgiHandle::epollCheck(int pipe_out)
{
    struct epoll_event ev;
    std::memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = pipe_out;
    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, pipe_out, &ev) == -1)
    {
        std::cerr << "Error: epoll_ctl failed" << std::endl;
        return 0;
    }
    return 1;
}

void CgiHandle::closePipe()
{
    if (_pipeIn[0] != -1)
        close(_pipeIn[0]);
    if (_pipeIn[1] != -1)
        close(_pipeIn[1]);
    if (_pipeOut[0] != -1)
        close(_pipeOut[0]);
    if (_pipeOut[1] != -1)
        close(_pipeOut[1]);
}

std::string CgiHandle::getIp()
{
    std::string ip = std::to_string(_config->getHost()); // Convert in_addr_t to string
    std::stringstream ss;
    std::getline(ss, ip, ':');
    return ip;
}

int CgiHandle::getPipeIn()
{
    return _pipeIn[1];
}

int CgiHandle::getPipeOut()
{
    return _pipeOut[0];
}

int CgiHandle::getExitStatus()
{
    return _exitStatus;
}

int CgiHandle::getLength()
{
    return _length;
}

int CgiHandle::getPid()
{
    return _pid;
}

void CgiHandle::erasseLength(int length)
{
    _length -= length;
}
