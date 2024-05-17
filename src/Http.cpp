/**
 * @file Http.cpp
 * @brief Implementation of the Http class and its related classes and methods.
 */
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 17:49:32 by eseferi           #+#    #+#             */
/*   Updated: 2024/05/17 10:34:56 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http.hpp"
#include "defines.h"
#include <vector>
#include "utils.hpp"

// METHODS

// this function converts the HTTP Method enum to a string
std::string		HTTP::methodToString(Method method) {
	static const std::map<Method, std::string> methods = {
		{GET, "GET"},
		{HEAD, "HEAD"},
		{POST, "POST"},
		{PUT, "PUT"},
		{DELETE, "DELETE"},
		{TRACE, "TRACE"},
		{OPTIONS, "OPTIONS"},
        {CONNECT, "CONNECT"},
        {PATCH, "PATCH"}
	};
	std::map<Method, std::string>::const_iterator it = methods.find(method);
	if (it != methods.end())
		return it->second;
	else
		return "UNKNOWN";
}

// this function converts the HTTP Method string to an enum
HTTP::Method	HTTP::stringToMethod(const std::string& method) {
	static const std::map<std::string, Method> methods = {
        {"GET", GET},
        {"HEAD", HEAD},
        {"POST", POST},
        {"PUT", PUT},
        {"DELETE", DELETE},
        {"TRACE", TRACE},
        {"OPTIONS", OPTIONS},
        {"CONNECT", CONNECT},
        {"PATCH", PATCH}
	};
	std::map<std::string, Method>::const_iterator it = methods.find(method);
	if (it != methods.end())
		return it->second;
	else
		throw std::invalid_argument("Invalid Http Method: " + method);
}

// this function converts the HTTP Version enum to a string
std::string		HTTP::versionToString(Version version) {
	static const std::map<Version, std::string> versions = {
		{HTTP::HTTP_1_0, "HTTP/1.0"},
		{HTTP::HTTP_1_1, "HTTP/1.1"},
		{HTTP::HTTP_2_0, "HTTP/2.0"}
	};
	std::map<Version, std::string>::const_iterator it = versions.find(version);
	if (it != versions.end()) {
		return it->second;
	} else {
		return "UNKNOWN";
	}
}

// this function converts the HTTP Version string to an enum
HTTP::Version HTTP::stringToVersion(const std::string& version) {
    static const std::map<std::string, Version> versions = {
        {"HTTP/1.0", HTTP::HTTP_1_0},
        {"HTTP/1.1", HTTP::HTTP_1_1},
        {"HTTP/2.0", HTTP::HTTP_2_0}
    };
    auto it = versions.find(version);
    if (it != versions.end()) {
        return it->second;
    } else {
        throw std::invalid_argument("Invalid Http Version: " + version);
    }
}




// CLASSES
/*                HEADER                  */

HTTP::Header::Header() : _key(0), _value(0) {};
HTTP::Header::Header(const std::string &key, const std::string &value) : _key(key), _value(value) {};

// seters
void	HTTP::Header::setKey(const std::string &key) { this->_key = key; }
void	HTTP::Header::setValue(const std::string &value) { this->_value = value; }

// getters
const std::string	&HTTP::Header::getKey() const { return this->_key; }
const std::string	&HTTP::Header::getValue() const { return this->_value; }

// overload
bool	HTTP::Header::operator==(const std::string &key) const { return this->_key == key; }


// methods
std::string		HTTP::Header::serialize() const {
	std::string header;
	header += this->_key;
	header += ": ";
	header += this->_value;
	header += LINE_END;
	return header;
}

HTTP::Header	HTTP::Header::deserialize(const std::string &header) {
	std::vector<std::string> segments = utils::split(header, ": ");
	if (segments.size() < 2)
		throw std::runtime_error("Invalid HTTP Header: " + header);
	const std::string key = segments[0];
	const std::string value = segments[1];
	return Header(key, value);
}

/*                HEADER                  */

/*                REQUEST                 */

// Constructors
HTTP::Request::Request() : _method(HTTP::GET), _resource(0), _headers(), _version(HTTP::HTTP_1_1) {};
HTTP::Request::Request(Method method, const std::string& resource, const std::map<std::string, Header>& headers, Version version) : _method(method), _resource(resource), _headers(headers), _version(version) {};

// Setters
void	HTTP::Request::setMethod(Method method) { this->_method = method; }
void	HTTP::Request::setResource(const std::string &resource) { this->_resource = resource; }
void	HTTP::Request::setVersion(Version version) { this->_version = version; }
void	HTTP::Request::setHeaders(const std::map<std::string, Header> &headers) { this->_headers = headers; }

// Getters
HTTP::Version HTTP::Request::getVersion() const { return this->_version; }
HTTP::Method HTTP::Request::getMethod() const { return this->_method; }
const	std::string &HTTP::Request::getResource() const { return this->_resource; }
const	std::map<std::string, HTTP::Header> &HTTP::Request::getHeaders() const { return this->_headers; }

// Methods
std::string		HTTP::Request::serialize() const {
	std::string request;
	request += methodToString(this->_method);
	request += " ";
	request += this->_resource;
	request += " ";
	request += versionToString(this->_version);
	request += LINE_END;
	for (std::map<std::string, HTTP::Header>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
    	request += it->second.serialize();
	request += LINE_END;
	return request;
}

HTTP::Request	HTTP::Request::deserialize(const std::string &request) {
	std::vector<std::string> lines = utils::split(request, std::string(LINE_END));
	if (lines.size() < 1)
		throw std::runtime_error("HTTP Request ('" + std::string(request) + "') consisted of " + utils::toString(lines.size()) + " lines, should be >= 1.");
	if (lines[0].empty())
		throw std::runtime_error("First line of HTTP Request ('" + std::string(request) + "') was empty.");
	std::vector<std::string> segments = utils::split(lines[0], " ");
	if (segments.size() != 3)
		throw std::runtime_error("First line of HTTP Request ('" + std::string(request) + "') consisted of " + utils::toString(segments.size()) + " space separated segments, should be 3.");
	const Method method = stringToMethod(segments[0]);
	const std::string resource = segments[1];
	const Version version = stringToVersion(segments[2]);
	std::map<std::string, Header> headers;
	for (size_t i = 1; i < lines.size(); ++i) {
		if (lines[i].size() > 0) {
			const Header header = Header::deserialize(lines[i]);
			headers.insert(std::make_pair(header.getKey(), header));
		}
	}
	return Request(method, resource, headers, version);
}

//             REQUEST                 */


//            RESPONSE                 */

// Constructors
HTTP::Response::Response() : _responseCode(0), _version(HTTP::HTTP_1_1), _headers(), _body(0) {};

HTTP::Response::Response(int responseCode, Version version, const std::map<std::string, Header> &headers, const std::string &body) : _responseCode(responseCode), _version(version), _headers(headers), _body(body) {};

// Setters
void	HTTP::Response::setResponseCode(int responseCode) { this->_responseCode = responseCode; }
void	HTTP::Response::setVersion(Version version) { this->_version = version; }
void	HTTP::Response::setHeaders(const std::map<std::string, Header> &headers) { this->_headers = headers; }
void	HTTP::Response::setBody(const std::string &body) { this->_body = body; }

// Getters
int		HTTP::Response::getResponseCode() const { return this->_responseCode; }
HTTP::Version HTTP::Response::getVersion() const { return this->_version; }
const std::map<std::string, HTTP::Header>	&HTTP::Response::getHeaders() const { return this->_headers; }
const std::string &HTTP::Response::getBody() const { return this->_body; }

// Methods
std::string		HTTP::Response::serialize() const {
	std::string response;
	response += versionToString(this->_version);
	response += " ";
	response += utils::toString(this->_responseCode);
	response += LINE_END;
	for (std::map<std::string, HTTP::Header>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
    	response += it->second.serialize();
	response += LINE_END;
	response += this->_body;
	return response;
}

HTTP::Response		HTTP::Response::deserialize(const std::string &response) {
	std::vector<std::string> segments = utils::split(response, std::string(LINE_END) + std::string(LINE_END));
	if (segments.size() < 2)
		throw std::runtime_error("Invalid HTTP response: " + response);
	std::string headerSegment = segments[0];
	segments.erase(segments.begin());
	std::string body = utils::concat(segments, LINE_END);
	std::vector<std::string> headerLines = utils::split(headerSegment, std::string(LINE_END));
	const std::string& responseCodeLine = headerLines[0];
	std::vector<std::string> responseCodeSegments = utils::split(responseCodeLine, " ");
	Version version = stringToVersion(responseCodeSegments[0]);
	int responseCode = utils::strToInt(responseCodeSegments[1]);
	headerLines.erase(headerLines.begin());
	std::map<std::string, Header> headers;
	for (std::vector<std::string>::const_iterator it = headerLines.begin(); it != headerLines.end(); ++it) {
        const Header header = Header::deserialize(*it);
        headers.insert(std::make_pair(header.getKey(), header));
    }
	return Response(responseCode, version, headers, body);
}
