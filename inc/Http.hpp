/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 17:44:46 by eseferi           #+#    #+#             */
/*   Updated: 2024/06/24 18:46:52 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef HTTP_HPP
#define HTTP_HPP

#include "Webserv.hpp"

namespace   HTTP {
	// ENUMS
	

	class Request {
	private:
		std::string							_path; // Stores the path of the request URL
		std::string							_query; // Stores the query part of the request URL
		std::string							_fragment; // Stores the fragment part of the request URL (added for URI parsing)
		std::map<std::string, std::string>	_headers; // Maps header names to their values
		std::vector<u_int8_t>				_body; // Stores the body of the request as a vector of bytes (changed from string)
		std::string							_boundary;
		Method								_method; // Stores the HTTP method of the request
		std::map<u_int8_t, std::string>		_methodStr; // Maps numeric method identifiers to their string representations (added for convenience)
		ParsingState						_state; // Tracks the current state of request parsing
		size_t								_maxBodySize; // Maximum allowed size for the request body
		size_t								_bodyLength; // Current length of the request body
		short								_errorCode; // Stores an error code if the request parsing fails
		size_t								_chunkLength; // Length of the current chunk being processed (for chunked transfer encoding)
		std::string                     	_storage;
		std::string							_keyStorage; // Temporary storage for multipart/form-data keys
		short								_methodIndex; // Index of the current method being processed
		u_int8_t							_verMajor; // Major version number of the HTTP protocol
		u_int8_t							_verMinor; // Minor version number of the HTTP protocol
		std::string							_serverName; // Name of the server handling the request
		std::string							_bodyStr; // String representation of the request body (for convenience)
		
		/* Flags */
		bool	_fieldsDoneFlag; // Indicates whether all fields have been parsed
		bool	_bodyFlag; // Indicates whether the body is expected in the request
		bool	_bodyDoneFlag; // Indicates whether the body has been fully received and parsed
		bool	_completeFlag; // Indicates whether the request has been fully parsed and is ready to be processed
		bool	_chunkedFlag; // Indicates whether the request uses chunked transfer encoding
		bool	_multiformFlag; // Indicates whether the request is a multipart/form-data request
		
		void	_handleHeaders(); // Internal method to process headers

	public:
		Request(); // Default constructor
		// Request(Method method, const std::string& resource, const std::map<std::string, Header>& headers, Version version, const std::string& body); // Parameterized constructor
		~Request() {}; // Destructor

		// Getters
		Method										&getMethod(); // Returns the HTTP method of the request
		std::string									&getPath(); // Returns the path of the request URL
		std::string									&getQuery(); // Returns the query part of the request URL
		std::string									&getFragment(); // Returns the fragment part of the request URL
		const std::map<std::string, std::string>    &getHeaders() const;// Returns all headers as a string
		std::string									&getBody(); // Returns the body of the request
		std::string									&getBoundary(); // Returns the boundary string for multipart/form-data requests
		std::string									getHeader(const std::string &); // Returns the value of a specified header
		std::string									getMethodStr(); // Returns the string representation of the HTTP method
		std::string									getServerName(); // Returns the name of the server handling the request
		bool										getMultiformFlag(); // Returns true if the request is a multipart/form-data request

		// Setters
		void				setMethod(Method &); // Sets the HTTP method of the request
		void				setHeader(std::string &, std::string &); // Sets a header value
		void				setMaxBodySize(size_t); // Sets the maximum allowed size for the request body
		void				setBody(std::string name); // Sets the body of the request
		
		// Methods
		void				feed(char *data, size_t size); // Processes incoming data
		bool				isComplete(); // Returns true if the request has been fully parsed
		void				printMessage(); // Prints the request for debugging purposes
		void				clear(); // Clears the request object for reuse
		short				errorCode(); // Returns the error code if parsing failed
		bool				keepAlive(); // Returns true if the connection should be kept alive
		void				cutReqBody(int bytes); // Removes a specified number of bytes from the beginning of the request body
	};
    class   Response {
        private:
			ServerConfig			_server;
			std::string				_targetFile;
			std::vector<u_int8_t>	_body;
			size_t					_bodyLength;
			std::string				_responseBody;
			std::string				_location;
			short			 		_code;
			char			  		*_res;
			int						_cgi;
			int						_cgiFd[2];
			size_t					_cgiResponseLength;
			bool			  		_autoIndex;

			// Private methods

			int     buildBody();
			void    setStatusLine();
			void    setHeaders();
			void    setServerDefaultErrorPages();
			int     readFile();
			void    contentType();
			void    contentLength();
			void    connection();
			void    server();
			void    location();
			void    date();
			int     handleTarget();
			void    buildErrorBody();
			bool    reqError();
			int     handleCgi(std::string &);
			int     handleCgiTemp(std::string &);
			
		public:
			static Mime _mime;
			Response();
			Response(HTTP::Request&);
			// Response(StatusCode responseCode, Version version, const std::map<std::string, Header> &headers, const std::string &body);
			~Response() {};

			// Public members
			CgiHandler		_cgiObj;
			
			// Getters
			std::string     getRes();
			size_t      	getLen() const;
			int        		getCode() const;

			// Setters
			void        	setRequest(HTTP::Request &);
			void        	setServer(ServerConfig &);

			// methods
			void			buildResponse();
			void			clear();
			void			cutRes(size_t);
			int				getCgiState();
			void			setCgiState(int);
			void			setErrorResponse(short code);
			std::string		removeBoundary(std::string &body, std::string &boundary);
			std::string		_responseContent;
			HTTP::Request	_request;
    };
}


#endif // HTTP_HPP