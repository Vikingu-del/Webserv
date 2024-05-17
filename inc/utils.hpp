/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 18:29:47 by eseferi           #+#    #+#             */
/*   Updated: 2024/05/16 20:40:06 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <stdexcept>

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
    int strToInt(const std::string& str);
}