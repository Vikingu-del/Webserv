/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 17:44:46 by eseferi           #+#    #+#             */
/*   Updated: 2024/05/17 13:41:11 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <iostream>
#include <map>
#include "defines.h"
#include <vector>
#include "utils.hpp"

namespace   HTTP
{
	// ENUMS
	enum	StatusCode {
		OK = 200,
		CREATED = 201,
		ACCEPTED = 202,
		NO_CONTENT = 204,
		MOVED_PERMANENTLY = 301,
		FOUND = 302,
		SEE_OTHER = 303,
		NOT_MODIFIED = 304,
		BAD_REQUEST = 400,
		UNAUTHORIZED = 401,
		FORBIDDEN = 403,
		NOT_FOUND = 404,
		METHOD_NOT_ALLOWED = 405,
		REQUEST_TIMEOUT = 408,
		INTERNAL_SERVER_ERROR = 500,
		NOT_IMPLEMENTED = 501,
		BAD_GATEWAY = 502,
		SERVICE_UNAVAILABLE = 503
	};
	
	enum	Method
	{
		GET,
		HEAD,
		POST,
		PUT,
		DELETE,
		TRACE,
		OPTIONS,
		CONNECT,
		PATCH
	};

	enum Version
	{
		HTTP_1_0,
		HTTP_1_1,
		HTTP_2_0
	};

	// METHODS
	std::string	methodToString(Method method);
	Method		stringToMethod(const std::string& method);
	std::string	versionToString(Version version);
	Version		stringToVersion(const std::string& version);

    // CLASSES
	class	Header
	{
		private:
			std::string		_key;
			std::string		_value;
		public:
			Header();
			Header(const std::string &key, const std::string &value);
			~Header() {};
			
			// Setters
			void			setKey(const std::string &key);
			void			setValue(const std::string &value);

			// Getters
			const			std::string &getKey() const;
			const			std::string &getValue() const;

			// Overload
			bool			operator==(const std::string &key) const;

			// methods
			std::string		serialize() const;
			static Header	deserialize(const std::string &header);
	};

	class	Request
	{
		private:
			Method							_method;
			std::string						_resource;
			std::map<std::string, Header>	_headers;
			Version							_version;
		public:
			Request();
			Request(Method method, const std::string& resource, const std::map<std::string, Header>& headers, Version version);
			~Request() {};
			
			// copy assignment operator
			Request& operator=(const Request& other);

			// Setters
			void								setVersion(Version version);
			void								setMethod(Method method);
			void								setResource(const std::string &resource);
			void									setHeaders(const std::map<std::string, Header> &headers);

			// Getters
			Version								getVersion() const;
			Method								getMethod() const;
			const std::string					&getResource() const;
			const std::map<std::string, Header>	&getHeaders() const;

			// Methods
			std::string							serialize() const;
			static Request						deserialize(const std::string &request);
	};

    class   Response {
        private:
            StatusCode							_responseCode;
            Version								_version;
            std::map<std::string, Header>		_headers;
            std::string							_body;
        public:
            Response();
            Response(StatusCode responseCode, Version version, const std::map<std::string, Header> &headers, const std::string &body);
            ~Response() {};
            
			// copy assignment operator
			Response& operator=(const Response& other);

            // Setters
            void								setResponseCode(StatusCode responseCode);
            void								setVersion(Version version);
            void								setHeaders(const std::map<std::string, Header> &headers);
            void								setBody(const std::string &body);

            // Getters
            StatusCode							getResponseCode() const;
            Version								getVersion() const;
            const std::map<std::string, Header>	&getHeaders() const;
            const std::string					&getBody() const;
            
            // Methods
            std::string							serialize() const;
            static Response						deserialize(const std::string &response);
    };
}
