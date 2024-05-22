/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 18:57:30 by eseferi           #+#    #+#             */
/*   Updated: 2024/05/22 17:46:52 by eseferi          ###   ########.fr       */
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
    std::cout << "statusCodeString" << std::endl;
    switch (statusCode) {
        case 200: return STATUS_200;
        case 201: return STATUS_201;
        case 202: return STATUS_202;
        case 203: return STATUS_203;
        case 204: return STATUS_204;
        case 205: return STATUS_205;
        case 206: return STATUS_206;
        case 300: return STATUS_300;
        case 301: return STATUS_301;
        case 302: return STATUS_302;
        case 303: return STATUS_303;
        case 304: return STATUS_304;
        case 307: return STATUS_307;
        case 308: return STATUS_308;
        case 400: return STATUS_400;
        case 401: return STATUS_401;
        case 403: return STATUS_403;
        case 404: return STATUS_404;
        case 405: return STATUS_405;
        case 406: return STATUS_406;
        case 407: return STATUS_407;
        case 408: return STATUS_408;
        case 409: return STATUS_409;
        case 410: return STATUS_410;
        case 411: return STATUS_411;
        case 412: return STATUS_412;
        case 413: return STATUS_413;
        case 414: return STATUS_414;
        case 415: return STATUS_415;
        case 416: return STATUS_416;
        case 417: return STATUS_417;
        case 418: return STATUS_418;
        case 421: return STATUS_421;
        case 425: return STATUS_425;
        case 426: return STATUS_426;
        case 428: return STATUS_428;
        case 429: return STATUS_429;
        case 431: return STATUS_431;
        case 451: return STATUS_451;
        case 500: return STATUS_500;
        case 501: return STATUS_501;
        case 502: return STATUS_502;
        case 503: return STATUS_503;
        case 504: return STATUS_504;
        case 505: return STATUS_505;
        case 506: return STATUS_506;
        case 507: return STATUS_507;
        case 510: return STATUS_510;
        case 511: return STATUS_511;
        default: return  STATUS_UNDEFINED;
    }
}