/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandle.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kilchenk <kilchenk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 15:46:46 by kilchenk          #+#    #+#             */
/*   Updated: 2024/06/07 14:50:24 by kilchenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HANDLE_HPP
# define CGI_HANDLE_HPP

#include "Webserv.hpp"
#include "ServerConfig.hpp"
#include <sys/epoll.h>

class ServerConfig;
class HTTP::Request;

class CgiHandle
{
    private:
        std::map<std::string, std::string> _env;
        ServerConfig *_config;
        HTTP::Request _req;
        std::string _pathStr;
        std::string _cgiReq; //cgi extansion
        pid_t       _pid;
        char        **_argv;
        char        **_envp;
        char        *_path;
        int         _pipeIn[2];
        int         _pipeOut[2];
        int         _epollFd;
        int         _length;
        int         _exitStatus;
    public:
        CgiHandle(ServerConfig *config, std::string extansion, int epollFd, HTTP::Request &req);
        ~CgiHandle();
        CgiHandle(const CgiHandle &copy);
        CgiHandle &operator=(const CgiHandle &copy);
        //getters
        std::string getIp();
        std::string getPath();
        int        getPipeIn();
        int        getPipeOut();
        int        getExitStatus();
        int        getLength();
        int        getPid();
        //setters
        void        setPath();
        void        setArgv();
        int         setPipe();
        //other
        std::string checkAccepting();// #! things
        void        initEnv();
        void        createArray();
        void        execCgi();
        void        closePipe();
        // void        subLength();
        
        int         epollCheck(int pipe_out);
        void        erasseLength(int length);
};

#endif