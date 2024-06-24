/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 18:29:47 by eseferi           #+#    #+#             */
/*   Updated: 2024/06/24 17:11:26 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"

namespace utils
{
    std::vector<std::string> split(const std::string &s, const std::string &delimiter);
    std::string concat(const std::vector<std::string> &v, const std::string &delimiter);
    template <typename T>
    std::string toString(T value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }
    unsigned int hexToDec(const std::string& nb);
    int strToInt(const std::string& str);
    std::string intToString(int value);
    std::string toLower(const std::string& str);
    std::string	statusCodeString(short statusCode);
    std::string getCurrentDateTime();
    void printMap(const std::map<std::string, std::string> &map);
    std::string getErrorPage(short statusCode);
    int buildHtmlIndex(std::string &dir_name, std::vector<u_int8_t> &body, size_t &body_len);
}