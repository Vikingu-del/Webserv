/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CofigFileUtils.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:46:14 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/20 16:37:51 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "../../inc/configFile/ConfigFileUtils.hpp"
#include "Webserv.hpp"

//! Because stoi is not available in c++98
int ft_stoi(std::string str)
{
    std::stringstream ss(str);
    if (str.length() > 10)
        throw std::exception();
    for (size_t i = 0; i < str.length(); ++i)
    {
        if(!isdigit(str[i]))
            throw std::exception();
    }
    int res;
    ss >> res;
    return (res);
}

std::string statusCodeString(short statusCode) {
    switch (statusCode) {
        case 100: return STATUS_100;
        case 101: return STATUS_101;
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
        default: return STATUS_UNDEFINED;
    }
}