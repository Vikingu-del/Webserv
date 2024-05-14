/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 17:44:46 by eseferi           #+#    #+#             */
/*   Updated: 2024/05/14 19:00:16 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include "Webserv.hpp"

namespace HTTP
{
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

    std::string methodToString(Method method);

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

    class 
}