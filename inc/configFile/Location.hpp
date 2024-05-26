/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 15:51:09 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/26 18:01:05 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "ConfigFile.hpp"
#include <vector>
#include <map>
#include <string>
#include <exception>
#include "utils.hpp"

class Location 
{
	// !Private Attributes 
	private:
		std::string					_path;
		std::string					_root;
		bool						_autoindex;
		std::string					_index;
		std::string					_return;
		std::string					_alias;
		unsigned long				_client_max_body_size;
		std::vector<short>			_methods;
		std::vector<std::string>	_cgi_path;
		std::vector<std::string>	_cgi_ext;

	// !Public Attributes
	public:
		std::map<std::string, std::string>	_ext_path;

	// !Constructors / Destructor
	public:
		Location();
		Location(const Location & other);
		Location	&operator=(const Location & other);
		~Location();

	// !Getters
	public:
		const std::string							&getPath() const;
		const std::string							&getRootLocation() const;
		const bool									&getAutoindex() const;
		const std::string							&getIndexLocation() const;
		const std::vector<short> 					&getMethods() const;
		const std::string 							&getReturn() const;
		const std::string 							&getAlias() const;
		const std::vector<std::string>				&getCgiPath() const;
		const std::vector<std::string>				&getCgiExtension() const; 
		const std::map<std::string, std::string>	&getExtensionPath() const;
		const unsigned long							&getMaxBodySize() const;
		std::string									getPrintMethods() const;

	// !Setters
	public:
		void	setPath(std::string value);
		void	setRootLocation(std::string value);
		void	setMethods(std::vector<std::string> methods);
		void	setAutoindex(std::string value);
		void	setIndexLocation(std::string value);
		void	setReturn(std::string value);
		void	setAlias(std::string value);		
		void	setCgiPath(std::vector<std::string> path);
		void	setCgiExtension(std::vector<std::string> extension);
		void	setMaxBodySize(std::string string_value);
		void	setMaxBodySize(unsigned long value);
};

#endif