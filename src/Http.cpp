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

// METHODS

// this function converts the HTTP Method enum to a string
std::string		HTTP::methodToString(Method method) {
	static const std::pair<Method, std::string> methodPairs[] = {
		std::make_pair(GET, "GET"), std::make_pair(HEAD, "HEAD"), std::make_pair(POST, "POST"), 
		std::make_pair(PUT, "PUT"), std::make_pair(DELETE, "DELETE"), std::make_pair(TRACE, "TRACE"), 
		std::make_pair(OPTIONS, "OPTIONS"), std::make_pair(CONNECT, "CONNECT"), std::make_pair(PATCH, "PATCH")
	};
	static const std::map<Method, std::string> methods(methodPairs, methodPairs + sizeof(methodPairs) / sizeof(methodPairs[0]));
	std::map<Method, std::string>::const_iterator it = methods.find(method);
	if (it != methods.end()) return it->second;
	return "UNKNOWN";
}

// this function converts the HTTP Method string to an enum
HTTP::Method	HTTP::stringToMethod(const std::string& method) {
	static const std::pair<std::string, Method> methodPairs[] = {
		std::make_pair("GET", GET), std::make_pair("HEAD", HEAD), std::make_pair("POST", POST), std::make_pair("PUT", PUT),
		std::make_pair("DELETE", DELETE), std::make_pair("TRACE", TRACE), std::make_pair("OPTIONS", OPTIONS),
		std::make_pair("CONNECT", CONNECT), std::make_pair("PATCH", PATCH)
	};
	static const std::map<std::string, Method> methods(methodPairs, methodPairs + sizeof(methodPairs) / sizeof(methodPairs[0]));
	std::map<std::string, Method>::const_iterator it = methods.find(method);
	if (it != methods.end()) return it->second;
	throw std::invalid_argument("Invalid HTTP Method: " + method);
}

// this function converts the HTTP Version enum to a string
std::string		HTTP::versionToString(Version version) {
	static const std::pair<Version, std::string> versionPairs[] = {
		std::make_pair(HTTP_1_0, "HTTP/1.0"), std::make_pair(HTTP_1_1, "HTTP/1.1"), std::make_pair(HTTP_2_0, "HTTP/2.0")
	};
	static const std::map<Version, std::string> versions(versionPairs, versionPairs + sizeof(versionPairs) / sizeof(versionPairs[0]));
	std::map<Version, std::string>::const_iterator it = versions.find(version);
	if (it != versions.end()) return it->second;
	return "UNKNOWN";
}

// this function converts the HTTP Version string to an enum
HTTP::Version	HTTP::stringToVersion(const std::string& version) {
	static const std::pair<std::string, Version> versionPairs[] = {
		std::make_pair("HTTP/1.0", HTTP_1_0), std::make_pair("HTTP/1.1", HTTP_1_1), std::make_pair("HTTP/2.0", HTTP_2_0)
	};
	static const std::map<std::string, Version> versions(versionPairs, versionPairs + sizeof(versionPairs) / sizeof(versionPairs[0]));
	std::map<std::string, Version>::const_iterator it = versions.find(version);
	if (it != versions.end()) return it->second;
	throw std::invalid_argument("Invalid HTTP Version: " + version);
}

// this function convers the RequestHeaderType enum to a string
std::string HTTP::headerTypeToStr(HTTP::HeaderType header)
{
	static const std::pair<HTTP::HeaderType, std::string> headerPairs[] = {
		std::make_pair(HTTP::ACCEPT, "Accept"), std::make_pair(HTTP::ACCEPT_CHARSET, "Accept-Charset"), std::make_pair(HTTP::ACCEPT_ENCODING, "Accept-Encoding"),
		std::make_pair(HTTP::ACCEPT_LANGUAGE, "Accept-Language"), std::make_pair(HTTP::AUTHORIZATION, "Authorization"), std::make_pair(HTTP::EXPECT, "Expect"),
		std::make_pair(HTTP::FROM, "From"), std::make_pair(HTTP::HOST, "Host"), std::make_pair(HTTP::IF_MATCH, "If-Match"),
		std::make_pair(HTTP::IF_MODIFIED_SINCE, "If-Modified-Since"), std::make_pair(HTTP::IF_NONE_MATCH, "If-None-Match"), std::make_pair(HTTP::IF_RANGE, "If-Range"),
		std::make_pair(HTTP::IF_UNMODIFIED_SINCE, "If-Unmodified-Since"), std::make_pair(HTTP::MAX_FORWARDS, "Max-Forwards"), std::make_pair(HTTP::PROXY_AUTHORIZATION, "Proxy-Authorization"),
		std::make_pair(HTTP::RANGE, "Range"), std::make_pair(HTTP::REFERER, "Referer"), std::make_pair(HTTP::TE, "TE"),
		std::make_pair(HTTP::USER_AGENT, "User-Agent"), std::make_pair(HTTP::ACCEPT_RANGES, "Accept-Ranges"), std::make_pair(HTTP::AGE, "Age"),
		std::make_pair(HTTP::ETAG, "ETag"), std::make_pair(HTTP::LOCATION, "Location"), std::make_pair(HTTP::PROXY_AUTHENTICATE, "Proxy-Authenticate"),
		std::make_pair(HTTP::RETRY_AFTER, "Retry-After"), std::make_pair(HTTP::SERVER, "Server"), std::make_pair(HTTP::VARY, "Vary"),
		std::make_pair(HTTP::WWW_AUTHENTICATE, "WWW-Authenticate"), std::make_pair(HTTP::NONE, "NONE"),
		std::make_pair(HTTP::CACHE_CONTROL, "Cache-Control"), std::make_pair(HTTP::CONNECTION, "Connection"),
		std::make_pair(HTTP::DATE, "Date"), std::make_pair(HTTP::PRAGMA, "Pragma"), std::make_pair(HTTP::TRAILER, "Trailer"),
		std::make_pair(HTTP::TRANSFER_ENCODING, "Transfer-Encoding"), std::make_pair(HTTP::UPGRADE, "Upgrade"), std::make_pair(HTTP::VIA, "Via"),
		std::make_pair(HTTP::WARNING, "Warning"), std::make_pair(HTTP::ALLOW, "Allow"), std::make_pair(HTTP::CONTENT_ENCODING, "Content-Encoding"),
		std::make_pair(HTTP::CONTENT_LANGUAGE, "Content-Language"), std::make_pair(HTTP::CONTENT_LENGTH, "Content-Length"), std::make_pair(HTTP::CONTENT_LOCATION, "Content-Location"),
		std::make_pair(HTTP::CONTENT_MD5, "Content-MD5"), std::make_pair(HTTP::CONTENT_RANGE, "Content-Range"), std::make_pair(HTTP::CONTENT_TYPE, "Content-Type"),
		std::make_pair(HTTP::LAST_MODIFIED, "Last-Modified")
	};
	static const std::map<HTTP::HeaderType, std::string> headers(headerPairs, headerPairs + sizeof(headerPairs) / sizeof(headerPairs[0]));
	std::map<HTTP::HeaderType, std::string>::const_iterator it = headers.find(header);
	if (it != headers.end()) return it->second;
	return "UNKNOWN";
}

// this function converts the RequestHeaderType string to an enum
HTTP::HeaderType HTTP::strToHeaderType(const std::string& header)
{
	static const std::pair<std::string, HTTP::HeaderType> headerPairs[] = {
		std::make_pair("Accept", HTTP::ACCEPT), std::make_pair("Accept-Charset", HTTP::ACCEPT_CHARSET), std::make_pair("Accept-Encoding", HTTP::ACCEPT_ENCODING),
		std::make_pair("Accept-Language", HTTP::ACCEPT_LANGUAGE), std::make_pair("Authorization", HTTP::AUTHORIZATION), std::make_pair("Expect", HTTP::EXPECT),
		std::make_pair("From", HTTP::FROM), std::make_pair("Host", HTTP::HOST), std::make_pair("If-Match", HTTP::IF_MATCH),
		std::make_pair("If-Modified-Since", HTTP::IF_MODIFIED_SINCE), std::make_pair("If-None-Match", HTTP::IF_NONE_MATCH), std::make_pair("If-Range", HTTP::IF_RANGE),
		std::make_pair("If-Unmodified-Since", HTTP::IF_UNMODIFIED_SINCE), std::make_pair("Max-Forwards", HTTP::MAX_FORWARDS), std::make_pair("Proxy-Authorization", HTTP::PROXY_AUTHORIZATION),
		std::make_pair("Range", HTTP::RANGE), std::make_pair("Referer", HTTP::REFERER), std::make_pair("TE", HTTP::TE),
		std::make_pair("User-Agent", HTTP::USER_AGENT), std::make_pair("Accept-Ranges", HTTP::ACCEPT_RANGES), std::make_pair("Age", HTTP::AGE),
		std::make_pair("ETag", HTTP::ETAG), std::make_pair("Location", HTTP::LOCATION), std::make_pair("Proxy-Authenticate", HTTP::PROXY_AUTHENTICATE),
		std::make_pair("Retry-After", HTTP::RETRY_AFTER), std::make_pair("Server", HTTP::SERVER), std::make_pair("Vary", HTTP::VARY),
		std::make_pair("WWW-Authenticate", HTTP::WWW_AUTHENTICATE), std::make_pair("NONE", HTTP::NONE), std::make_pair("Cache-Control", HTTP::CACHE_CONTROL),
        std::make_pair("Connection", HTTP::CONNECTION), std::make_pair("Date", HTTP::DATE), std::make_pair("Pragma", HTTP::PRAGMA),
		std::make_pair("Trailer", HTTP::TRAILER), std::make_pair("Transfer-Encoding", HTTP::TRANSFER_ENCODING), std::make_pair("Upgrade", HTTP::UPGRADE),
		std::make_pair("Via", HTTP::VIA), std::make_pair("Warning", HTTP::WARNING), std::make_pair("Allow", HTTP::ALLOW),
		std::make_pair("Content-Encoding", HTTP::CONTENT_ENCODING), std::make_pair("Content-Language", HTTP::CONTENT_LANGUAGE), std::make_pair("Content-Length", HTTP::CONTENT_LENGTH),
		std::make_pair("Content-Location", HTTP::CONTENT_LOCATION), std::make_pair("Content-MD5", HTTP::CONTENT_MD5), std::make_pair("Content-Range", HTTP::CONTENT_RANGE),
		std::make_pair("Content-Type", HTTP::CONTENT_TYPE), std::make_pair("Last-Modified", HTTP::LAST_MODIFIED)
	};
	static const std::map<std::string, HTTP::HeaderType> headers(headerPairs, headerPairs + sizeof(headerPairs) / sizeof(headerPairs[0]));
	std::map<std::string, HTTP::HeaderType>::const_iterator it = headers.find(header);
	if (it != headers.end()) return it->second;
	return HTTP::NONE;

}


// CLASSES
/*                HEADER                  */

HTTP::Header::Header() : _key(""), _value(""), _type(NONE) {}
HTTP::Header::Header(const std::string &key, const std::string &value) : _key(key), _value(value), _type(strToHeaderType(key)) {}

// seters
void	HTTP::Header::setKey(const std::string &key) { _key = key; _type = strToHeaderType(key);}
void	HTTP::Header::setValue(const std::string &value) { _value = value; }
void	HTTP::Header::setType(HeaderType type) { _type = type; }

// getters
const std::string		&HTTP::Header::getKey() const { return _key; }
const std::string		&HTTP::Header::getValue() const { return _value; }
const HTTP::HeaderType	&HTTP::Header::getType() const { return _type; }

// overload
bool	HTTP::Header::operator==(const std::string &key) const { return _key == key; }
bool	HTTP::Header::operator==(const HTTP::HeaderType &header) const { return _type == header; }


// methods
std::string		HTTP::Header::serialize() const {
	return _key + ": " + _value + LINE_END;
}

HTTP::Header	HTTP::Header::deserialize(const std::string &header) {
	std::vector<std::string> segments = utils::split(header, ": ");
	if (segments.size() < 2)
		throw std::runtime_error("Invalid HTTP Header: " + header);
	return Header(segments[0], segments[1]);
}

/*                HEADER                  */

/*                REQUEST                 */

// Constructors
HTTP::Request::Request()
	: _method(HTTP::GET), _resource(0), _headers(), _version(HTTP::HTTP_1_1), _body(0) {};
HTTP::Request::Request(Method method, const std::string& resource, const std::map<std::string, HTTP::Header>& headers, Version version, const std::string& body)
    : _method(method), _resource(resource), _headers(headers), _version(version), _body(body) {}

// copy assignment operator
HTTP::Request& HTTP::Request::operator=(const HTTP::Request& other) {
    if (this != &other) {
        _method = other._method;
        _resource = other._resource;
        _headers = other._headers;
        _version = other._version;
    }
    return *this;
}

// Setters
void	HTTP::Request::setMethod(Method method) { _method = method; }
void	HTTP::Request::setResource(const std::string &resource) { _resource = resource; }
void	HTTP::Request::setVersion(Version version) { _version = version; }
void	HTTP::Request::setHeaders(const std::map<std::string, Header> &headers) { _headers = headers; }
void	HTTP::Request::setBody(const std::string &body) { _body = body; }

// Getters
HTTP::Version	HTTP::Request::getVersion() const { return _version; }
HTTP::Method	HTTP::Request::getMethod() const { return _method; }
const			std::string &HTTP::Request::getResource() const { return _resource; }
const			std::map<std::string, HTTP::Header> &HTTP::Request::getHeaders() const { return _headers; }
const			std::string &HTTP::Request::getBody() const { return _body; }

// Methods
std::string		HTTP::Request::serialize() const {
	std::string request = methodToString(_method) + " " + _resource + " " + versionToString(_version) + LINE_END;
	for (std::map<std::string, HTTP::Header>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
    	request += it->second.serialize();
	request += LINE_END;
	request += _body;
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
	size_t i;
	for (i = 1; i < lines.size(); ++i) {
		if (lines[i].size() > 0) {
			const Header header = Header::deserialize(lines[i]);
			headers.insert(std::make_pair(header.getKey(), header));
		} else
			break; // stop when we reach the blank line separating headers from body
	}
	std::string body;
	for (++i; i < lines.size(); ++i)
		body += lines[i] + LINE_END;
	return Request(method, resource, headers, version, body);
}

//             REQUEST                 */


//            RESPONSE                 */

// Constructors
HTTP::Response::Response() : _responseCode(), _version(HTTP::HTTP_1_1), _headers(), _body("") {};

HTTP::Response::Response(StatusCode responseCode, Version version, const std::map<std::string, Header>& headers, const std::string& body)
	: _responseCode(responseCode), _version(version), _headers(headers), _body(body) {};

// copy assignment operator
HTTP::Response& HTTP::Response::operator=(const HTTP::Response& other) {
	if (this != &other) {
		_responseCode = other._responseCode;
		_version = other._version;
		_headers = other._headers;
		_body = other._body;
	}
	return *this;
}


// Setters
void	HTTP::Response::setResponseCode(StatusCode responseCode) { _responseCode = responseCode; }
void	HTTP::Response::setVersion(Version version) { _version = version; }
void	HTTP::Response::setHeaders(const std::map<std::string, Header> &headers) { _headers = headers; }
void	HTTP::Response::setBody(const std::string &body) { _body = body; }

// Getters
HTTP::StatusCode	HTTP::Response::getResponseCode() const { return _responseCode; }
HTTP::Version		HTTP::Response::getVersion() const { return _version; }
const				std::map<std::string, HTTP::Header>	&HTTP::Response::getHeaders() const { return _headers; }
const				std::string &HTTP::Response::getBody() const { return _body; }

// Methods
std::string		HTTP::Response::serialize() const {
	std::string response = versionToString(_version) + " " + utils::toString(_responseCode) + LINE_END;
	for (std::map<std::string, HTTP::Header>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
    	response += it->second.serialize();
	response += LINE_END;
	response += _body;
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
	return Response(StatusCode(responseCode), version, headers, body);
}
