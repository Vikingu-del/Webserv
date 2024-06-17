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

std::string HTTP::methodToString(Method method) {
    static const std::pair<Method, std::string> methodPairs[] = {
        std::pair<Method, std::string>(GET, "GET"),
        std::pair<Method, std::string>(HEAD, "HEAD"),
        std::pair<Method, std::string>(POST, "POST"),
        std::pair<Method, std::string>(PUT, "PUT"),
        std::pair<Method, std::string>(DELETE, "DELETE"),
        std::pair<Method, std::string>(TRACE, "TRACE"),
        std::pair<Method, std::string>(OPTIONS, "OPTIONS"),
        std::pair<Method, std::string>(CONNECT, "CONNECT"),
        std::pair<Method, std::string>(PATCH, "PATCH")
    };
    static const std::map<Method, std::string> methods(
        methodPairs, methodPairs + sizeof(methodPairs) / sizeof(methodPairs[0])
    );
    std::map<Method, std::string>::const_iterator it = methods.find(method);
    if (it != methods.end()) return it->second;
    return "UNKNOWN";
}

HTTP::Method HTTP::stringToMethod(const std::string& method) {
    static const std::pair<std::string, Method> methodPairs[] = {
        std::pair<std::string, Method>("GET", GET),
        std::pair<std::string, Method>("HEAD", HEAD),
        std::pair<std::string, Method>("POST", POST),
        std::pair<std::string, Method>("PUT", PUT),
        std::pair<std::string, Method>("DELETE", DELETE),
        std::pair<std::string, Method>("TRACE", TRACE),
        std::pair<std::string, Method>("OPTIONS", OPTIONS),
        std::pair<std::string, Method>("CONNECT", CONNECT),
        std::pair<std::string, Method>("PATCH", PATCH)
    };
    static const std::map<std::string, Method> methods(
        methodPairs, methodPairs + sizeof(methodPairs) / sizeof(methodPairs[0])
    );
    std::map<std::string, Method>::const_iterator it = methods.find(method);
    if (it != methods.end()) return it->second;
    throw std::invalid_argument("Invalid HTTP Method: " + method);
}

// this function converts the HTTP Version enum to a string
std::string HTTP::versionToString(Version version) {
    static const std::pair<Version, std::string> versionPairs[] = {
        std::pair<Version, std::string>(HTTP_1_0, "HTTP/1.0"),
        std::pair<Version, std::string>(HTTP_1_1, "HTTP/1.1"),
        std::pair<Version, std::string>(HTTP_2_0, "HTTP/2.0")
    };
    static const std::map<Version, std::string> versions(
        versionPairs, versionPairs + sizeof(versionPairs) / sizeof(versionPairs[0])
    );
    std::map<Version, std::string>::const_iterator it = versions.find(version);
    if (it != versions.end()) return it->second;
    return "UNKNOWN";
}

// this function converts the HTTP Version string to an enum
HTTP::Version HTTP::stringToVersion(const std::string& version) {
    static const std::pair<std::string, Version> versionPairs[] = {
        std::pair<std::string, Version>("HTTP/1.0", HTTP_1_0),
        std::pair<std::string, Version>("HTTP/1.1", HTTP_1_1),
        std::pair<std::string, Version>("HTTP/2.0", HTTP_2_0)
    };
    static const std::map<std::string, Version> versions(
        versionPairs, versionPairs + sizeof(versionPairs) / sizeof(versionPairs[0])
    );
    std::map<std::string, Version>::const_iterator it = versions.find(version);
    if (it != versions.end()) return it->second;
    throw std::invalid_argument("Invalid HTTP Version: " + version);
}

// this function converts the HTTP StatusCode enum to a string
std::string HTTP::statusCodeToString(StatusCode statusCode) {
    static const std::pair<StatusCode, std::string> statusCodePairs[] = {
        std::pair<StatusCode, std::string>(OK, "200 OK"),
        std::pair<StatusCode, std::string>(MOVED_PERMANENTLY, "301 Moved Permanently"),
        std::pair<StatusCode, std::string>(MOVED_TEMPORARILY, "302 Moved Temporarily"),
        std::pair<StatusCode, std::string>(NOT_MODIFIED, "304 Not Modified"),
        std::pair<StatusCode, std::string>(BAD_REQUEST, "400 Bad Request"),
        std::pair<StatusCode, std::string>(UNAUTHORIZED, "401 Unauthorized"),
        std::pair<StatusCode, std::string>(FORBIDDEN, "403 Forbidden"),
        std::pair<StatusCode, std::string>(NOT_FOUND, "404 Not Found"),
        std::pair<StatusCode, std::string>(INTERNAL_SERVER_ERROR, "500 Internal Server Error"),
        std::pair<StatusCode, std::string>(NOT_IMPLEMENTED, "501 Not Implemented"),
        std::pair<StatusCode, std::string>(BAD_GATEWAY, "502 Bad Gateway"),
        std::pair<StatusCode, std::string>(SERVICE_UNAVAILABLE, "503 Service Unavailable")
    };
    static const std::map<StatusCode, std::string> statusCodes(
        statusCodePairs, statusCodePairs + sizeof(statusCodePairs) / sizeof(statusCodePairs[0])
    );
    std::map<StatusCode, std::string>::const_iterator it = statusCodes.find(statusCode);
    if (it != statusCodes.end()) return it->second;
    return "UNKNOWN";
}

// CLASSES
/* HEADER */

HTTP::Header::Header() : _key(""), _value("") {}
HTTP::Header::Header(const std::string &key, const std::string &value) : _key(key), _value(value) {}

// Setters
void HTTP::Header::setKey(const std::string &key) { _key = key; }
void HTTP::Header::setValue(const std::string &value) { _value = value; }

// Getters
const std::string &HTTP::Header::getKey() const { return _key; }
const std::string &HTTP::Header::getValue() const { return _value; }

// Overload
bool HTTP::Header::operator==(const std::string &key) const { return _key == key; }

// Methods
std::string HTTP::Header::serialize() const {
    return _key + ": " + _value + LINE_END;
}

HTTP::Header HTTP::Header::deserialize(const std::string &header) {
    std::vector<std::string> segments = utils::split(header, ": ");
    if (segments.size() < 2)
        throw std::runtime_error("Invalid HTTP Header: " + header);
    return Header(segments[0], segments[1]);
}

/*                HEADER                  */

/* REQUEST */

// Constructors
HTTP::Request::Request()
    : _method(HTTP::GET), _resource(""), _headers(), _version(HTTP::HTTP_1_1), _body(""), _uri(""), _query("") {};

HTTP::Request::Request(Method method, const std::string& resource, const std::map<std::string, HTTP::Header>& headers, Version version, const std::string& body)
    : _method(method), _resource(resource), _headers(headers), _version(version), _body(body) {
    parseQuery();
    parseUri();
}

// Copy assignment operator
HTTP::Request& HTTP::Request::operator=(const HTTP::Request& other) {
    if (this != &other) {
        _method = other._method;
        _resource = other._resource;
        _headers = other._headers;
        _version = other._version;
        _body = other._body;
        _uri = other._uri;
        _query = other._query;
    }
    return *this;
}

// Setters
void HTTP::Request::setMethod(Method method) { _method = method; }
void HTTP::Request::setResource(const std::string &resource) { _resource = resource; }
void HTTP::Request::setVersion(Version version) { _version = version; }
void HTTP::Request::setHeaders(const std::map<std::string, Header> &headers) { _headers = headers; }
void HTTP::Request::setBody(const std::string &body) { _body = body; }
void HTTP::Request::setUri(const std::string &uri) { _uri = uri; }
void HTTP::Request::setQuery(const std::string &query) { _query = query; }

// Getters
HTTP::Version HTTP::Request::getVersion() const { return _version; }
HTTP::Method HTTP::Request::getMethod() const { return _method; }
const std::string &HTTP::Request::getResource() const { return _resource; }
const std::map<std::string, HTTP::Header> &HTTP::Request::getHeaders() const { return _headers; }
const std::string &HTTP::Request::getBody() const { return _body; }
void HTTP::Request::getHeader(const std::string &key, std::string &src) const {
    std::map<std::string, Header>::const_iterator it = _headers.find(key);
    if (it != _headers.end()) src = it->second.getValue();
    else src = "";
}
const std::string &HTTP::Request::getUri() const { return _uri; }
const std::string &HTTP::Request::getQuery() const { return _query; }

void HTTP::Request::parseQuery() {
    std::size_t pos = _resource.find("?");
    if (pos != std::string::npos)
        _query = _resource.substr(pos + 1);
    else
        _query = "";
}

void HTTP::Request::parseUri() {
    std::size_t pos = _resource.find("?");
    if (pos != std::string::npos) 
        _uri = _resource.substr(0, pos);
    else
        _uri = _resource;
}

// Methods
std::string HTTP::Request::serialize() const {
    std::string request = methodToString(_method) + " " + _resource + " " + versionToString(_version) + LINE_END;
    for (std::map<std::string, HTTP::Header>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
        request += it->second.serialize();
    request += LINE_END;
    request += _body;
    return request;
}

HTTP::Request HTTP::Request::deserialize(const std::string &request) {
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


/* RESPONSE */

// Constructors
HTTP::Response::Response() : _responseCode(), _version(HTTP::HTTP_1_1), _headers(), _body("") {};

HTTP::Response::Response(StatusCode responseCode, Version version, const std::map<std::string, HTTP::Header>& headers, const std::string& body)
    : _responseCode(responseCode), _version(version), _headers(headers), _body(body) {};

// Copy assignment operator
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
void HTTP::Response::setResponseCode(StatusCode responseCode) { _responseCode = responseCode; }
void HTTP::Response::setVersion(Version version) { _version = version; }
void HTTP::Response::setHeaders(const std::map<std::string, Header> &headers) { _headers = headers; }
void HTTP::Response::setBody(const std::string &body) { _body = body; }

// Getters
HTTP::StatusCode HTTP::Response::getResponseCode() const { return _responseCode; }
HTTP::Version HTTP::Response::getVersion() const { return _version; }
const std::map<std::string, HTTP::Header> &HTTP::Response::getHeaders() const { return _headers; }
const std::string &HTTP::Response::getBody() const { return _body; }
void HTTP::Response::getHeader(const std::string &key, std::string &src) const {
    std::map<std::string, Header>::const_iterator it = _headers.find(key);
    if (it != _headers.end()) src = it->second.getValue();
    else src = "";
}

// Methods
std::string HTTP::Response::serialize() const {
    std::string response = versionToString(_version) + " " + statusCodeToString(_responseCode) + LINE_END;
    for (std::map<std::string, HTTP::Header>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
        response += it->second.serialize();
    response += LINE_END;
    response += _body;
    return response;
}

HTTP::Response HTTP::Response::deserialize(const std::string &response) {
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

void HTTP::Response::appendToBody(const std::string &data) {
    _body += data;
}
//            RESPONSE                 */
