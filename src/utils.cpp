/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 18:57:30 by eseferi           #+#    #+#             */
/*   Updated: 2024/06/15 18:23:36 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"


std::vector<std::string> utils::split(const std::string &s, const std::string &delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = s.find(delimiter);
    while (end != std::string::npos) {
        tokens.push_back(s.substr(start, end - start));
        start = end + delimiter.length();
        end = s.find(delimiter, start);
    }
    tokens.push_back(s.substr(start, end));
    return tokens;
}

std::string utils::concat(const std::vector<std::string> &v, const std::string &delimiter) {
    std::string s;
    for (size_t i = 0; i < v.size(); ++i) {
        s += v[i];
        if (i != v.size() - 1)
            s += delimiter;
    }
    return s;
}

int utils::strToInt(const std::string& str) {
    int result = 0;
    for (size_t i = 0; i < str.size(); ++i) {
        if (!std::isdigit(str[i]))
            throw std::invalid_argument("Invalid input string");
        result = result * 10 + str[i] - '0';
    }
    return result;
}

std::string utils::toLower(const std::string& str)
{
    std::string lowerStr = str;
    std::locale loc;
    for (std::string::size_type i = 0; i < lowerStr.length(); ++i)
    {
        lowerStr[i] = std::use_facet<std::ctype<char> >(loc).tolower(lowerStr[i]);
    }
    return lowerStr;
}

//! Returns the status code string
std::string utils::statusCodeString(short statusCode) {
    // std::cout << "statusCodeString" << std::endl;
    switch (statusCode) {
        case 400: return STATUS_400;
        case 403: return STATUS_403;
        case 404: return STATUS_404;
        case 405: return STATUS_405;
        case 500: return STATUS_500;
        case 505: return STATUS_505;
        default: return  STATUS_UNDEFINED;
    }
}

std::string utils::getCurrentDateTime() {
    std::time_t now = std::time(0);
    std::tm *gmt = std::gmtime(&now);
    
    char buffer[100];
    strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S GMT", gmt);
    return std::string(buffer);
}

std::string utils::intToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void utils::printMap(const std::map<std::string, std::string> &map) {
    std::map<std::string, std::string>::const_iterator it = map.begin();
    while (it != map.end()) {
        std::cout << it->first << " = " << it->second << std::endl;
        it++;
    }
}