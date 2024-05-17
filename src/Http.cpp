/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 17:49:32 by eseferi           #+#    #+#             */
/*   Updated: 2024/05/16 20:26:17 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http.hpp"
#include "defines.h"
#include <vector>
#include "utils.hpp"

// METHODS

std::string		HTTP::methodToString(Method method) {
	switch(method)
	{
		case HTTP::GET:
			return "GET";
		case HTTP::HEAD:
			return "HEAD";
		case HTTP::POST:
			return "POST";
		case HTTP::PUT:
			return "PUT";
		case HTTP::DELETE:
			return "DELETE";
		case HTTP::TRACE:
			return "TRACE";
		case HTTP::OPTIONS:
			return "OPTIONS";
		case HTTP::CONNECT:
			return "CONNECT";
		case HTTP::PATCH:
			return "PATCH";
		default:
			return "UNKNOWN";
	}
}

HTTP::Method	HTTP::stringToMethod(const std::string& method) {
	if (method == "GET")
		return HTTP::GET;
	else if (method == "HEAD")
		return HTTP::HEAD;
	else if (method == "POST")
		return HTTP::POST;
	else if (method == "PUT")
		return HTTP::PUT;
	else if (method == "DELETE")
		return HTTP::DELETE;
	else if (method == "TRACE")
		return HTTP::TRACE;
	else if (method == "OPTIONS")
		return HTTP::OPTIONS;
	else if (method == "CONNECT")
		return HTTP::CONNECT;
	else if (method == "PATCH")
		return HTTP::PATCH;
	else
		return HTTP::GET;
}

std::string		HTTP::versionToString(Version version) {
	switch(version)
	{
		case HTTP::HTTP_1_0:
			return "HTTP/1.0";
		case HTTP::HTTP_1_1:
			return "HTTP/1.1";
		case HTTP::HTTP_2_0:
			return "HTTP/2.0";
		default:
			return "UNKNOWN";
	}
}

HTTP::Version	HTTP::stringToVersion(const std::string& version) {
	if (version == "HTTP/1.0")
		return HTTP::HTTP_1_0;
	else if (version == "HTTP/1.1")
		return HTTP::HTTP_1_1;
	else if (version == "HTTP/2.0")
		return HTTP::HTTP_2_0;
	else
		return HTTP::HTTP_1_1;
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
	std::vector<std::string> segments = utils::split(header, " ");
	const std::string key = segments[0].substr(0, segments[0].size() - 1);
	segments.erase(segments.begin());
	const std::string value = utils::concat(segments, " ");
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
	for (size_t i = 0; i < headerLines.size(); ++i) {
		const Header header = Header::deserialize(headerLines[i]);
		headers.insert(std::make_pair(header.getKey(), header));
	}
	return Response(responseCode, version, headers, body);
}
