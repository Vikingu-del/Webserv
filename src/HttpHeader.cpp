/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHeader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 17:49:32 by eseferi           #+#    #+#             */
/*   Updated: 2024/05/14 18:54:05 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpParser.hpp"

/*                HEADER                  */

HTTP::Header::Header() : _key(0), _value(0) {};
HTTP::Header::Header(const std::string &key, const std::string &value) : _key(key), _value(value) {};

// seters
void HTTP::Header::setKey(const std::string &key) { this->_key = key; }
void HTTP::Header::setValue(const std::string &value) { this->_value = value; }

// getters
const std::string &HTTP::Header::getKey() const { return this->_key; }
const std::string &HTTP::Header::getValue() const { return this->_value; }

// overload
bool HTTP::Header::operator==(const std::string &key) const { return this->_key == key; }


// methods
std::string HTTP::Header::serialize() const {
    std::string header;
    header += this->_key;
    header += ": ";
    header += this->_value;
    header += LINE_END;
    return header;
}

HTTP::Header HTTP::Header::deserialize(const std::string &line) {
    std::vector<std::string> segments = utils::split(line, ' ');
    const std::string key = segments[0].substr(0, segments[0].size() - 1);
    segments.erase(segments.begin());
    const std::string value = utils::concat(segments, ' ');
    return Header(key, value);
}

/*                HEADER                  */



