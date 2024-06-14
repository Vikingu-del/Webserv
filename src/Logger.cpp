/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 16:52:12 by ipetruni          #+#    #+#             */
/*   Updated: 2024/06/05 14:09:34 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

LoggerState Logger::state = ON;
std::string Logger::file_name = "logfile.txt";

void Logger::logMsg(const char *color, LoggerMode m, const char* msg, ...) {
	if (state == ON) {
		char output[8192];
		va_list args;
		va_start(args, msg);
		vsnprintf(output, sizeof(output), msg, args);
		va_end(args);

		std::string date = getCurrTime();

		if (m == FILE_OUTPUT) {
			struct stat st;
			if (stat("logs", &st) != 0) {
				if (mkdir("logs", 0777) < 0 && errno != EEXIST) {
					std::cerr << "mkdir() Error: " << strerror(errno) << std::endl;
					return;
				}
			}

			try {
				std::ofstream log_file(("logs/" + file_name).c_str(), std::ios::app);
				if (!log_file) {
					throw std::ios_base::failure("Error opening file");
				}
				log_file << date << " " << output << "\n";
				log_file.close();
			} catch (const std::exception &e) {
				std::cerr << "File operation error: " << e.what() << std::endl;
			}
		} else if (m == CONSOLE_OUTPUT) {
			std::cout << color << date << output << "\x1B[0m" << std::endl;
		}
	}
}

std::string Logger::getCurrTime() {
	std::time_t now = std::time(NULL);
	struct tm *tm_info = std::localtime(&now);
	char date[100];
	std::strftime(date, sizeof(date), "[%Y-%m-%d %H:%M:%S] ", tm_info);
	return std::string(date);
}

void Logger::setFilenName(const std::string &name) {
	Logger::file_name = name;
}

void Logger::setState(LoggerState s) {
	Logger::state = s;
}
