/**
 * @file Http.cpp
 * @brief Implementation of the Http namespace and its related classes and methods.
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

static const std::pair<HTTP::HeaderType, std::string> headerPairs[] = {
	std::make_pair(HTTP::ACCEPT, "accept"), std::make_pair(HTTP::ACCEPT_CHARSET, "accept-charset"), std::make_pair(HTTP::ACCEPT_ENCODING, "accept-encoding"),
	std::make_pair(HTTP::ACCEPT_LANGUAGE, "accept-language"), std::make_pair(HTTP::AUTHORIZATION, "authorization"), std::make_pair(HTTP::EXPECT, "expect"),
	std::make_pair(HTTP::FROM, "from"), std::make_pair(HTTP::HOST, "host"), std::make_pair(HTTP::IF_MATCH, "if-match"),
	std::make_pair(HTTP::IF_MODIFIED_SINCE, "if-modified-since"), std::make_pair(HTTP::IF_NONE_MATCH, "if-none-match"), std::make_pair(HTTP::IF_RANGE, "if-range"),
	std::make_pair(HTTP::IF_UNMODIFIED_SINCE, "if-unmodified-since"), std::make_pair(HTTP::MAX_FORWARDS, "max-forwards"), std::make_pair(HTTP::PROXY_AUTHORIZATION, "proxy-authorization"),
	std::make_pair(HTTP::RANGE, "range"), std::make_pair(HTTP::REFERER, "referer"), std::make_pair(HTTP::TE, "te"),
	std::make_pair(HTTP::USER_AGENT, "user-agent"), std::make_pair(HTTP::ACCEPT_RANGES, "accept-ranges"), std::make_pair(HTTP::AGE, "age"),
	std::make_pair(HTTP::ETAG, "etag"), std::make_pair(HTTP::LOCATION, "location"), std::make_pair(HTTP::PROXY_AUTHENTICATE, "proxy-authenticate"),
	std::make_pair(HTTP::RETRY_AFTER, "retry-after"), std::make_pair(HTTP::SERVER, "server"), std::make_pair(HTTP::VARY, "vary"),
	std::make_pair(HTTP::WWW_AUTHENTICATE, "www-authenticate"), std::make_pair(HTTP::NONE, "none"),
	std::make_pair(HTTP::CACHE_CONTROL, "cache-control"), std::make_pair(HTTP::CONNECTION, "connection"),
	std::make_pair(HTTP::DATE, "date"), std::make_pair(HTTP::PRAGMA, "pragma"), std::make_pair(HTTP::TRAILER, "trailer"),
	std::make_pair(HTTP::TRANSFER_ENCODING, "transfer-encoding"), std::make_pair(HTTP::UPGRADE, "upgrade"), std::make_pair(HTTP::VIA, "via"),
	std::make_pair(HTTP::CONTENT_LANGUAGE, "content-language"), std::make_pair(HTTP::CONTENT_LENGTH, "content-length"), std::make_pair(HTTP::CONTENT_LOCATION, "content-location"),
	std::make_pair(HTTP::CONTENT_MD5, "content-md5"), std::make_pair(HTTP::CONTENT_RANGE, "content-range"), std::make_pair(HTTP::CONTENT_TYPE, "content-type"),
	std::make_pair(HTTP::LAST_MODIFIED, "last-modified")
};

static const std::map<HTTP::HeaderType, std::string> headers(headerPairs, headerPairs + sizeof(headerPairs) / sizeof(headerPairs[0]));

// this function convers the RequestHeaderType enum to a string
std::string HTTP::headerTypeToStr(HTTP::HeaderType header)
{
	std::map<HTTP::HeaderType, std::string>::const_iterator it = headers.find(header);
	if (it != headers.end()) return it->second;
	return "UNKNOWN";
}

// this function converts the RequestHeaderType string to an enum
HTTP::HeaderType HTTP::strToHeaderType(const std::string& header)
{
	std::string lowerHeader = utils::toLower(header);
	for (std::map<HTTP::HeaderType, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
		if (it->second == lowerHeader) return it->first;
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
	if (request.size() >= 8000)
		throw std::runtime_error("HTTP Request ('" + std::string(request) + "') was too long.");
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

//            RESPONSE                 */

//         REQUEST HANDLER             */

// std::map<std::string, std::pair<HTTP::Method, HTTP::Response(*)(/*const HTTP::Request&*/)> >& HTTP::getRoutes() {
//     static std::map<std::string, std::pair<Method, Response(*)(/*const Request&*/)> > routes;
//     return routes;
// }

// HTTP::Response HTTP::getHome(/*const HTTP::Request& req*/) {
// 	std::string body;
// 	std::ifstream file("gameHub/srcs/indexes/home.html");
// 	std::cout << "here is the file" << file.is_open() << std::endl;
// 	if (file.is_open()) {
// 		std::stringstream buffer;
// 		buffer << file.rdbuf();
// 		// std::cout << "\n here is the buffer" << buffer.str() << std::endl;
// 		body = buffer.str();
// 		return HTTP::Response(HTTP::OK, HTTP::HTTP_1_1, std::map<std::string, HTTP::Header>(), body);
// 	}
// 	return (HTTP::Response(HTTP::NOT_FOUND, HTTP::HTTP_1_1, std::map<std::string, HTTP::Header>(), ""));
// }

// void HTTP::initRoutes() {
//     std::map<std::string, std::pair<HTTP::Method, HTTP::Response(*)(/*const HTTP::Request&*/)> >& routes = HTTP::getRoutes();
//     routes.insert(std::make_pair("/", std::make_pair(HTTP::GET, getHome)));
//     routes.insert(std::make_pair("/home", std::make_pair(HTTP::GET, getHome)));
// }

// std::string	HTTP::handleRequest(const std::string &request/*, const ServerConfig &server*/) {
//     try {
//         // !1. Parse the request:
//         HTTP::Request req = HTTP::Request::deserialize(request);

// 		// !2. Determine the server:
// 		// Since we're passing a ServerConfig object to the function, we already have the server
// 		// information. We can use this information as needed in the following steps.

// 		// !3.Check the request against the server's configuration:
// 		// We might want to check if the request is valid according to the server's configuration.
// 		// For example, we could check if the request's target resource matches one of the server's
// 		// locations, if the request's body size is within the server's max body size, etc.

// 		// !4. Generate the response:
// 		// If the request is for a CGI script, you would run the script here, capture its output, and use that
// 		// to generate the response. If the request is not for a CGI script, you would generate the response as
// 		// you would normally.

// 		// !5. Serialize the response:
// 		// Use the HTTP::Response::serialize() method to convert the HTTP::Response object into a 
// 		// string that can be sent over the network.

//         // 2. Determine the response based on the parsed request:
//         HTTP::initRoutes();
//         std::map<std::string, std::pair<HTTP::Method, HTTP::Response(*)(/*const HTTP::Request&*/)> >& routes = HTTP::getRoutes();
//         std::map<std::string, std::pair<HTTP::Method, HTTP::Response(*)(/*const HTTP::Request&*/)> >::const_iterator it = routes.find(req.getResource());
//         if (it != routes.end() && it->second.first == req.getMethod()) {
//             HTTP::Response res = it->second.second(/*req*/);
//             return res.serialize();
//         } else
//             return HTTP::Response(HTTP::NOT_FOUND, HTTP::HTTP_1_1, std::map<std::string, HTTP::Header>(), "").serialize();
//     } catch (const std::exception &e) {
//         std::cerr << RED << "Error: " << e.what() << RESET << std::endl;
//     }
//     return Response(HTTP::INTERNAL_SERVER_ERROR, HTTP::HTTP_1_1, std::map<std::string, HTTP::Header>(), "").serialize();
// }