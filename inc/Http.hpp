/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 17:44:46 by eseferi           #+#    #+#             */
/*   Updated: 2024/05/16 20:03:21 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <iostream>
#include <map>

namespace   HTTP
{
    // Constatns
    const int OK = 200;
    const int CREATED = 201;
    const int ACCEPTED = 202;
    const int NO_CONTENT = 204;
    const int MOVED_PERMANENTLY = 301;
    const int FOUND = 302;
    const int SEE_OTHER = 303;
    const int NOT_MODIFIED = 304;
    const int BAD_REQUEST = 400;
    const int UNAUTHORIZED = 401;
    const int FORBIDDEN = 403;
    const int NOT_FOUND = 404;
    const int METHOD_NOT_ALLOWED = 405;
    const int REQUEST_TIMEOUT = 408;
    const int INTERNAL_SERVER_ERROR = 500;
    const int NOT_IMPLEMENTED = 501;
    const int BAD_GATEWAY = 502;
    const int SERVICE_UNAVAILABLE = 503;
    
    // ENUMS
    enum Method
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
    std::string methodToString(Method method);
    Method stringToMethod(const std::string& method);
    std::string versionToString(Version version);
    Version stringToVersion(const std::string& version);

    // CLASSES
    class   Header
    {
        private:
            std::string _key;
            std::string _value;

        public:
            Header();
            Header(const std::string &key, const std::string &value);
            ~Header() {};
            
            // Setters
            void setKey(const std::string &key);
            void setValue(const std::string &value);

            // Getters
            const std::string &getKey() const;
            const std::string &getValue() const;

            // Overload
            bool operator==(const std::string &key) const;

            // methods
            std::string serialize() const;
            static Header deserialize(const std::string &header);
    };

    class   Request
    {
        private:
            Method _method;
            std::string _resource;
            std::map<std::string, Header> _headers;
            Version _version;
        public:
            Request();
            Request(Method method, const std::string& resource, const std::map<std::string, Header>& headers, Version version);
            ~Request() {};

        // Setters
        void setVersion(Version version);
        void setMethod(Method method);
        void setResource(const std::string &resource);
        void setHeaders(const std::map<std::string, Header> &headers);

        // Getters
        Version getVersion() const;
        Method getMethod() const;
        const std::string &getResource() const;
        const std::map<std::string, Header> &getHeaders() const;

        // Methods
        std::string serialize() const;
        static Request deserialize(const std::string &request);
    };

    class   Response {
        private:
            int _responseCode;
            Version _version;
            std::map<std::string, Header> _headers;
            std::string _body;
        public:
            Response();
            Response(int responseCode, Version version, const std::map<std::string, Header> &headers, const std::string &body);
            ~Response() {};
            
            // Setters
            void setResponseCode(int responseCode);
            void setVersion(Version version);
            void setHeaders(const std::map<std::string, Header> &headers);
            void setBody(const std::string &body);

            // Getters
            int getResponseCode() const;
            Version getVersion() const;
            const std::map<std::string, Header> &getHeaders() const;
            const std::string &getBody() const;
            
            // Methods
            std::string serialize() const;
            static Response deserialize(const std::string &response);
    };
}