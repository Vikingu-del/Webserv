/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 18:57:30 by eseferi           #+#    #+#             */
/*   Updated: 2024/06/24 12:00:44 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>


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

unsigned int utils::hexToDec(const std::string& nb)
{
	unsigned int x;
	std::stringstream ss;
	ss << nb;
	ss >> std::hex >> x;
	return (x);
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
    switch (statusCode)
    {
        case 100:
            return "Continue";
        case 101:
            return "Switching Protocol";
        case 200:
            return "OK";
        case 201:
            return "Created";
        case 202:
            return "Accepted";
        case 203:
            return "Non-Authoritative Information";
        case 204:
            return "No Content";
        case 205:
            return "Reset Content";
        case 206:
            return "Partial Content";
        case 300:
            return "Multiple Choice";
        case 301:
            return "Moved Permanently";
        case 302:
            return "Moved Temporarily";
        case 303:
            return "See Other";
        case 304:
            return "Not Modified";
        case 307:
            return "Temporary Redirect";
        case 308:
            return "Permanent Redirect";
        case 400:
            return "Bad Request";
        case 401:
            return "Unauthorized";
        case 403:
            return "Forbidden";
        case 404:
            return "Not Found";
        case 405:
            return "Method Not Allowed";
        case 406:
            return "Not Acceptable";
        case 407:
            return "Proxy Authentication Required";
        case 408:
            return "Request Timeout";
        case 409:
            return "Conflict";
        case 410:
            return "Gone";
        case 411:
            return "Length Required";
        case 412:
            return "Precondition Failed";
        case 413:
            return "Payload Too Large";
        case 414:
            return "URI Too Long";
        case 415:
            return "Unsupported Media Type";
        case 416:
            return "Requested Range Not Satisfiable";
        case 417:
            return "Expectation Failed";
        case 418:
            return "I'm a teapot";
        case 421:
            return "Misdirected Request";
        case 425:
            return "Too Early";
        case 426:
            return "Upgrade Required";
        case 428:
            return "Precondition Required";
        case 429:
            return "Too Many Requests";
        case 431:
            return "Request Header Fields Too Large";
        case 451:
            return "Unavailable for Legal Reasons";
        case 500:
            return "Internal Server Error";
        case 501:
            return "Not Implemented";
        case 502:
            return "Bad Gateway";
        case 503:
            return "Service Unavailable";
        case 504:
            return "Gateway Timeout";
        case 505:
            return "HTTP Version Not Supported";
        case 506:
            return "Variant Also Negotiates";
        case 507:
            return "Insufficient Storage";
        case 510:
            return "Not Extended";
        case 511:
            return "Network Authentication Required";
        default:
            return "Undefined";
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

std::string utils::getErrorPage(short statusCode)
{
    return ("<html>\r\n<head><title>" + toString(statusCode) + " " +
             statusCodeString(statusCode) + " </title></head>\r\n" + "<body>\r\n" + 
            "<center><h1>" + toString(statusCode) + " " + statusCodeString(statusCode) + "</h1></center>\r\n");
}

int utils::buildHtmlIndex(std::string &dir_name, std::vector<u_int8_t> &body, size_t &body_len)
{
    struct dirent   *entityStruct;
    DIR             *directory;
    std::string     dirListPage;
    
    directory = opendir(dir_name.c_str());
    if (directory == NULL)
    {    
        std::cerr << "opendir failed" << std::endl;
        return (1);
    }
    dirListPage.append("<html>\n");
    dirListPage.append("<head>\n");
    dirListPage.append("<title> Index of");
    dirListPage.append(dir_name);
    dirListPage.append("</title>\n");
    dirListPage.append("</head>\n");
    dirListPage.append("<body >\n");
    dirListPage.append("<h1> Index of " + dir_name + "</h1>\n");
    dirListPage.append("<table style=\"width:80%; font-size: 15px\">\n");
    dirListPage.append("<hr>\n");
    dirListPage.append("<th style=\"text-align:left\"> File Name </th>\n");
    dirListPage.append("<th style=\"text-align:left\"> Last Modification  </th>\n");
    dirListPage.append("<th style=\"text-align:left\"> File Size </th>\n");

    struct stat file_stat;
    std::string file_path;

    while((entityStruct = readdir(directory)) != NULL)
    {
        if(strcmp(entityStruct->d_name, ".") == 0)
            continue;
        file_path = dir_name + entityStruct->d_name;
        stat(file_path.c_str() , &file_stat);
        dirListPage.append("<tr>\n");
        dirListPage.append("<td>\n");
        dirListPage.append("<a href=\"");
        dirListPage.append(entityStruct->d_name);
        if (S_ISDIR(file_stat.st_mode))
            dirListPage.append("/");
        dirListPage.append("\">");
        dirListPage.append(entityStruct->d_name);
        if (S_ISDIR(file_stat.st_mode))
            dirListPage.append("/");
        dirListPage.append("</a>\n");
        dirListPage.append("</td>\n");
        dirListPage.append("<td>\n");
        dirListPage.append(ctime(&file_stat.st_mtime));
        dirListPage.append("</td>\n");
        dirListPage.append("<td>\n");
        if (!S_ISDIR(file_stat.st_mode))
            dirListPage.append(utils::toString(file_stat.st_size));
        dirListPage.append("</td>\n");
        dirListPage.append("</tr>\n");
    }
    dirListPage.append("</table>\n");
    dirListPage.append("<hr>\n");

    dirListPage.append("</body>\n");
    dirListPage.append("</html>\n");

    body.insert(body.begin(), dirListPage.begin(), dirListPage.end());
    body_len = body.size();
    return (0);
}
