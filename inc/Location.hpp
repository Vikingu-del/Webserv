/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 15:51:09 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/10 17:10:31 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"

class Location 
{
	// !Private Attributes 
	private:
		std::string					_path;
		std::string					_root;
		bool						_autoindex;
		std::string					_index;
		std::vector<short>			_methods;
		std::string					_return;
		std::string					_alias;
		// TODO: 

		// std::vector<std::string>	_cgi_path;
		// std::vector<std::string>	_cgi_ext;
		
		// TODO: 
		unsigned long				_client_max_body_size;

	// !Public Attributes
	public:
		std::map<std::string, std::string> _ext_path;

	// !Constructors / Destructor
	public:
		Location();
		Location(const Location & other);
		Location &operator=(const Location & other);
		~Location();

	// !Getters
	public:
		const std::string &getPath() const;
		const std::string &getRootLocation() const;
		const bool &getAutoindex() const;
		const std::string &getIndexLocation() const;
		const std::vector<short> &getMethods() const;
		const std::string &getReturn() const;
		const std::string &getAlias() const;
		// TODO: 
		
		// const std::vector<std::string> &getCgiPath() const;
		// const std::vector<std::string> &getCgiExtension() const;

		// TODO: 
		const std::map<std::string, std::string> &getExtensionPath() const;
		const unsigned long &getMaxBodySize() const;

	// !Setters
	public:
		void setPath(std::string value);
		void setRootLocation(std::string value);
		void setMethods(std::vector<std::string> methods);
		void setAutoindex(std::string value);
		void setIndexLocation(std::string value);
		void setReturn(std::string value);
		void setAlias(std::string value);
		// TODO: 
		
		// void setCgiPath(std::vector<std::string> path);
		// void setCgiExtension(std::vector<std::string> extension);
		
		// TODO: 
		void setMaxBodySize(std::string string_value);
		void setMaxBodySize(unsigned long value);
};