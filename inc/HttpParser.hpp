/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 17:44:46 by eseferi           #+#    #+#             */
/*   Updated: 2024/05/14 20:27:45 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include "Webserv.hpp"

namespace HTTP
{
    // ENUMS
    enum class Method
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

    enum class Version
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
    class Header
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
            static Header deserialize(const std::string &line);
    };

    class  Request
    {
        private:
            Version _version;
            Method _method;
            std::string _resource;
            std::map<std::string, Header> _headers;
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
}