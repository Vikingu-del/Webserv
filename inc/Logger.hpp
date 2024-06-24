/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 16:47:38 by ipetruni          #+#    #+#             */
/*   Updated: 2024/06/24 18:44:01 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Webserv.hpp"


enum LogPrio{
    DEBUG,
    INFO,
    ERROR
};

enum L_State{
    ON,
    OFF
};


enum Mode{
    CONSOLE_OUTPUT,
    FILE_OUTPUT
};

class Logger{

    public:
        static std::string file_name;
        static LogPrio prio;
        static std::map<LogPrio, std::string> prio_str;
        static L_State state;

        static void         setFilenName(std::string);
        static void         setState(L_State);

        static void         logMsg(const char *, Mode, const char*, ...);
        static void         setPrio(LogPrio);
        static void         enableFileLog();
        static std::string  getCurrTime();
    private:
        static std::map<LogPrio, std::string> initMap();

};


#endif