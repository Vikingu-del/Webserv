/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 16:47:38 by ipetruni          #+#    #+#             */
/*   Updated: 2024/06/03 17:01:09 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cstdarg>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>



enum LoggerState {
	ON,
	OFF
};

enum LoggerMode {
	CONSOLE_OUTPUT,
	FILE_OUTPUT
};

enum LoggerMsgPriority {
	DEBUG,
	INFO,
	ERROR
};

class Logger {
	public:
		static std::string file_name;
		static LoggerState state;

		static void setFilenName(const std::string&);
		static void setState(LoggerState);

		static void logMsg(const char*, LoggerMode, const char*, ...);
		static void setPrio(LoggerMsgPriority);
		static void enableFileLog();
		static std::string getCurrTime();

	private:
		static std::map<LoggerMsgPriority, std::string> initMap();
};

#endif
