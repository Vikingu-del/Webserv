/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 18:57:30 by eseferi           #+#    #+#             */
/*   Updated: 2024/05/16 20:38:14 by eseferi          ###   ########.fr       */
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