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

#include "Webserv.hpp"

HTTP::Request::Request()
{
    _methodStr[::HTTP::GET] = "GET";
    _methodStr[::HTTP::POST] = "POST";
    _methodStr[::HTTP::DELETE] = "DELETE";
    _methodStr[::HTTP::PUT] = "PUT";
    _methodStr[::HTTP::HEAD] = "HEAD";
    _path = "";
    _query = "";
    _fragment = "";
    _bodyStr = "";
    _errorCode = 0;
    _chunkLength = 0;
    _method = NONE;
    _methodIndex = 1;
    _state = Request_Line;
    _fieldsDoneFlag = false;
    _bodyFlag = false;
    _bodyDoneFlag = false;
    _chunkedFlag = false;
    _bodyLength = 0;
    _storage = "";
    _keyStorage = "";
    _multiformFlag = false;
    _boundary = "";
}

HTTP::Request::~Request() {}

bool    checkUriPos(std::string path)
{
    std::string tmp(path);
    char *res = strtok((char*)tmp.c_str(), "/");
    int pos = 0;
    while (res != NULL)
    {
        if (!strcmp(res, ".."))
            pos--;
        else
            pos++;
        if (pos < 0)
            return (1);
        res = strtok(NULL, "/");
    }
    return (0);
}

/**

 * Checks if character is allowed to be in a URI
 * Characters allowed as specifed in RFC:
   Alphanumeric: A-Z a-z 0-9
   Unreserved: - _ . ~
   Reserved:  * ' ( ) ; : @ & = + $ , / ? % # [ ]

 **/
bool    allowedCharURI(u_int8_t ch)
{
    if ((ch >= '#' && ch <= ';') || (ch >= '?' && ch <= '[') || (ch >= 'a' && ch <= 'z') ||
       ch == '!' || ch == '=' || ch == ']' || ch == '_' || ch == '~')
        return (true);
    return (false);
}

/**

* Checks whether the character passed is allowed in a field name
* Characters allowed as specifed in RFC:

"!" / "#" / "$" / "%" / "&" / "'"
/ "*" / "+" / "-" / "." / "^" / "_"
/ "`" / "|" / "~" / 0-9 / A-Z / a-z

**/
bool    isToken(u_int8_t ch)
{
    if (ch == '!' || (ch >= '#' && ch <= '\'') || ch == '*'|| ch == '+' || ch == '-'  || ch == '.' ||
       (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || (ch >= '^' && ch <= '`') ||
       (ch >= 'a' && ch <= 'z') || ch == '|')
        return (true);
    return (false);
}

/* Trim leading and trailing  spaces */
void    trimStr(std::string &str)
{
    static const char* spaces = " \t";
    str.erase(0, str.find_first_not_of(spaces));
    str.erase(str.find_last_not_of(spaces) + 1);
}

void    toLower(std::string &str)
{
    for (size_t i = 0; i < str.length(); ++i)
        str[i] = std::tolower(str[i]);
}

void    HTTP::Request::feed(char *data, size_t size)
{
    u_int8_t character;
    static std::stringstream s;

    for (size_t i = 0; i < size; ++i)
    {
        character = data[i];
        switch (_state)
        {
            case Request_Line:
            {
                if (character == 'G')
                    _method = GET;
                else if (character == 'P')
                {
                    _state = Request_Line_Post_Put;
                    break ;
                }
                else if (character == 'D')
                    _method = DELETE;
                else if (character == 'H')
                    _method = HEAD;
                else
                {
                    _errorCode = 501;
                    std::cout << "Method Error Request_Line and Character is = " << character << std::endl;
                    return ;
                }
                _state = Request_Line_Method;
                break ;
            }
            case Request_Line_Post_Put:
            {
                if (character == 'O')
                    _method = POST;
                else if (character == 'U')
                    _method = PUT;
                else
                {
                    _errorCode = 501;
                    std::cout << "Method Error Request_Line and Character is = " << character << std::endl;
                    return ;
                }
                _methodIndex++;
                _state = Request_Line_Method;
                break ;
            }
            case Request_Line_Method:
            {
                if (character == _methodStr[_method][_methodIndex])
                    _methodIndex++;
                else
                {
                    _errorCode = 501;
                    std::cout << "Method Error Request_Line and Character is = " << character << std::endl;
                    return ;
                }

                if ((size_t) _methodIndex == _methodStr[_method].length())
                    _state = Request_Line_First_Space;
                break ;
            }
            case Request_Line_First_Space:
            {
                if (character != ' ')
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
                    return ;
                }
                _state = Request_Line_URI_Path_Slash;
                continue ;
            }
            case Request_Line_URI_Path_Slash:
            {
                if (character == '/')
                {
                    _state = Request_Line_URI_Path;
                    _storage.clear();
                }
                else
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Request_Line_URI_Path_Slash)" << std::endl;
                    return ;
                }
                break ;
            }
            case Request_Line_URI_Path:
            {
                if (character == ' ')
                {
                    _state = Request_Line_Ver;
                    _path.append(_storage);
                    _storage.clear();
                    continue ;
                }
                else if (character == '?')
                {
                    _state = Request_Line_URI_Query;
                    _path.append(_storage);
                    _storage.clear();
                    continue ;
                }
                else if (character == '#')
                {
                    _state = Request_Line_URI_Fragment;
                    _path.append(_storage);
                    _storage.clear();
                    continue ;
                }
                else if (!allowedCharURI(character))
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Request_Line_URI_Path)" << std::endl;
                    return ;
                }
                else if ( i > MAX_URI_LENGTH)
                {
                    _errorCode = 414;
                    std::cout << "URI Too Long (Request_Line_URI_Path)" << std::endl;
                    return ;
                }
                break ;
            }
            case Request_Line_URI_Query:
            {
                if (character == ' ')
                {
                    _state = Request_Line_Ver;
                    _query.append(_storage);
                    _storage.clear();
                    continue ;
                }
                else if (character == '#')
                {
                    _state = Request_Line_URI_Fragment;
                    _query.append(_storage);
                    _storage.clear();
                    continue ;
                }
                else if (!allowedCharURI(character))
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Request_Line_URI_Query)" << std::endl;
                    return ;
                }
                else if ( i > MAX_URI_LENGTH)
                {
                    _errorCode = 414;
                    std::cout << "URI Too Long (Request_Line_URI_Path)" << std::endl;
                    return ;
                }
                break ;
            }
            case Request_Line_URI_Fragment:
            {
                if (character == ' ')
                {
                    _state = Request_Line_Ver;
                    _fragment.append(_storage);
                    _storage.clear();
                    continue ;
                }
                else if (!allowedCharURI(character))
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Request_Line_URI_Fragment)" << std::endl;
                    return ;
                }
                else if ( i > MAX_URI_LENGTH)
                {
                    _errorCode = 414;
                    std::cout << "URI Too Long (Request_Line_URI_Path)" << std::endl;
                    return ;
                }
                break ;
            }
            case Request_Line_Ver:
            {
                if (checkUriPos(_path))
                {
                    _errorCode = 400;
                    std::cout << "Request URI ERROR: goes before root !!" << std::endl;
                    return ;
                }
                if (character != 'H')
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Request_Line_Ver)" << std::endl;
                    return ;
                }
                _state = Request_Line_HT;
                break ;
            }
            case Request_Line_HT:
            {
                if (character != 'T')
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Request_Line_HT)" << std::endl;
                    return ;
                }
                _state = Request_Line_HTT;
                break ;
            }
            case Request_Line_HTT:
            {
                if (character != 'T')
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Request_Line_HTT)" << std::endl;
                    return ;
                }
                _state = Request_Line_HTTP;
                break ;
            }
            case Request_Line_HTTP:
            {
                if (character != 'P')
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Request_Line_HTTP)" << std::endl;
                    return ;
                }
                _state = Request_Line_HTTP_Slash;
                break ;
            }
            case Request_Line_HTTP_Slash:
            {
                if (character != '/')
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Request_Line_HTTP_Slash)" << std::endl;
                    return ;
                }
                _state = Request_Line_Major;
                break ;
            }
            case Request_Line_Major:
            {
                if (!isdigit(character))
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Request_Line_Major)" << std::endl;
                    return ;
                }
                _verMajor = character;

                _state = Request_Line_Dot;
                break ;
            }
            case Request_Line_Dot:
            {
                if (character != '.')
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Request_Line_Dot)" << std::endl;
                    return ;
                }
                _state = Request_Line_Minor;
                break ;
            }
            case Request_Line_Minor:
            {
                if (!isdigit(character))
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Request_Line_Minor)" << std::endl;
                    return ;
                }
                _verMinor = character;
                _state = Request_Line_CR;
                break ;
            }
            case Request_Line_CR:
            {
                if (character != '\r')
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Request_Line_CR)" << std::endl;
                    return ;
                }
                _state = Request_Line_LF;
                break ;
            }
            case Request_Line_LF:
            {
                if (character != '\n')
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Request_Line_LF)" << std::endl;
                    return ;
                }
                _state = Field_Name_Start;
                _storage.clear();
                continue ;
            }
            case Field_Name_Start:
            {
                if (character == '\r')
                    _state = Fields_End;
                else if (isToken(character))
                    _state = Field_Name;
                else
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Field_Name_Start)" << std::endl;
                    return ;
                }
                break ;
            }
            case Fields_End:
            {
                if (character == '\n')
                {
                    _storage.clear();
                    _fieldsDoneFlag = true;
                    _handleHeaders();
                    //if no body then parsing is completed.
                    if (_bodyFlag == 1)
                    {
                        if (_chunkedFlag == true)
                            _state = Chunked_Length_Begin;
                        else
                        {
                            _state = Message_Body;
                        }
                    }
                    else
                    {
                        _state = Parsing_Done;
                    }
                    continue ;
                }
                else
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Fields_End)" << std::endl;
                    return ;
                }
                break ;
            }
            case Field_Name:
            {
                if (character == ':')
                {
                    _keyStorage = _storage;
                    _storage.clear();
                    _state = Field_Value;
                    continue ;
                }
                else if (!isToken(character))
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Field_Name)" << std::endl;
                    return ;
                }
                break ;
                //if (!character allowed)
                // error;
            }
            case Field_Value:
            {
                if ( character == '\r' )
                {
                    setHeader(_keyStorage, _storage);
                    _keyStorage.clear();
                    _storage.clear();
                    _state = Field_Value_End;
                    continue ;
                }
                break ;
            }
            case Field_Value_End:
            {
                if ( character == '\n' )
                {
                    _state = Field_Name_Start;
                    continue ;
                }
                else
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Field_Value_End)" << std::endl;
                    return ;
                }
                break ;
            }
            case Chunked_Length_Begin:
            {
                if (isxdigit(character) == 0)
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Chunked_Length_Begin)" << std::endl;
                    return ;
                }
                s.str("");
                s.clear();
                s << character;
                s >> std::hex >> _chunkLength;
                if (_chunkLength == 0)
                    _state = Chunked_Length_CR;
                else
                    _state = Chunked_Length;
                continue ;
            }
            case Chunked_Length:
            {
                if (isxdigit(character) != 0)
                {
                    int temp_len = 0;
                    s.str("");
                    s.clear();
                    s << character;
                    s >> std::hex >> temp_len;
                    _chunkLength *= 16;
                    _chunkLength += temp_len;
                }
                else if (character == '\r')
                    _state = Chunked_Length_LF;
                else
                    _state = Chunked_Ignore;
                continue ;
            }
            case Chunked_Length_CR:
            {
                if ( character == '\r')
                    _state = Chunked_Length_LF;
                else
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Chunked_Length_CR)" << std::endl;
                    return ;
                }
                continue ;
            }
            case Chunked_Length_LF:
            {
                if ( character == '\n')
                {
                    if (_chunkLength == 0)
                        _state = Chunked_End_CR;
                    else
                        _state = Chunked_Data;
                }
                else
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Chunked_Length_LF)" << std::endl;
                    return ;
                }
                continue ;
            }
            case Chunked_Ignore:
            {
                if (character == '\r')
                    _state = Chunked_Length_LF;
                continue ;
            }
            case Chunked_Data:
            {
				_body.push_back(character);
				--_chunkLength;
                if (_chunkLength == 0)
                    _state = Chunked_Data_CR;
				continue ;
            }
            case Chunked_Data_CR:
            {
                if ( character == '\r')
                    _state = Chunked_Data_LF;
                else
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Chunked_Data_CR)" << std::endl;
                    return ;
                }
                continue ;
            }
            case Chunked_Data_LF:
            {
                if ( character == '\n')
                    _state = Chunked_Length_Begin;
                else
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Chunked_Data_LF)" << std::endl;
                    return ;
                }
                continue ;
            }
            case Chunked_End_CR:
            {
                if (character != '\r')
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Chunked_End_CR)" << std::endl;
                    return ;
                }
                _state = Chunked_End_LF;
                continue ;

            }
            case Chunked_End_LF:
            {
                if (character != '\n')
                {
                    _errorCode = 400;
                    std::cout << "Bad Character (Chunked_End_LF)" << std::endl;
                    return ;
                }
                _bodyDoneFlag = true;
                _state = Parsing_Done;
                continue ;
            }
            case Message_Body:
            {
                if (_body.size() < _bodyLength )
                    _body.push_back(character);
                if (_body.size() == _bodyLength )
                {
                    _bodyDoneFlag = true;
                    _state = Parsing_Done;
                }
                break ;
            }
            case Parsing_Done:
            {
                return ;
            }
        }//end of swtich
        _storage += character;
    }
    if( _state == Parsing_Done)
    {
        _bodyStr.append((char*)_body.data(), _body.size());
    }
}

bool    HTTP::Request::isComplete()
{
    return (_state == Parsing_Done);
}

HTTP::Method&   HTTP::Request::getMethod()
{
    return (_method);
}

std::string& HTTP::Request::getPath()
{
    return (_path);
}

std::string &HTTP::Request::getQuery()
{
    return (_query);
}

std::string &HTTP::Request::getFragment()
{
    return (_fragment);
}

std::string HTTP::Request::getHeader(std::string const &name)
{
    return (_headers[name]);
}

const std::map<std::string, std::string>	&HTTP::Request::getHeaders() const
{
	return (this->_headers);
}

std::string HTTP::Request::getMethodStr()
{
	return (_methodStr[_method]);
}

std::string &HTTP::Request::getBody()
{
    return (_bodyStr);
}

std::string     HTTP::Request::getServerName()
{
    return (this->_serverName);
}

bool    HTTP::Request::getMultiformFlag()
{
    return (this->_multiformFlag);
}

std::string     &HTTP::Request::getBoundary()
{
    return (this->_boundary);
}

void    HTTP::Request::setBody(std::string body)
{
    _body.clear();
    _body.insert(_body.begin(), body.begin(), body.end());
    _bodyStr = body;
}

void    HTTP::Request::setMethod(Method & method)
{
    _method = method;
}

void    HTTP::Request::setHeader(std::string &name, std::string &value)
{
    trimStr(value);
    toLower(name);
    _headers[name] = value;
}

void    HTTP::Request::setMaxBodySize(size_t size)
{
    _maxBodySize = size;
}


void        HTTP::Request::printMessage()
{
    std::cout << _methodStr[_method] + " " + _path + "?" + _query + "#" + _fragment
              + " " + "HTTP/" << _verMajor  << "." << _verMinor << std::endl;

    for (std::map<std::string, std::string>::iterator it = _headers.begin();
    it != _headers.end(); ++it)
    {
        std::cout << it->first + ":" + it->second << std::endl;
    }
    for (std::vector<u_int8_t>::iterator it = _body.begin(); it != _body.end(); ++it)
    {
        std::cout << *it;
    }
    std::cout << std::endl << "END OF BODY" << std::endl;

    std::cout << "BODY FLAG =" << _bodyFlag << "  _BOD_done_flag= " << _bodyDoneFlag << "FEIDLS FLAG = " << _fieldsDoneFlag
    << std::endl;
}


void        HTTP::Request::_handleHeaders()
{
    std::stringstream ss;

    if (_headers.count("content-length"))
    {
        _bodyFlag = true;
        ss << _headers["content-length"];
        ss >> _bodyLength;
    }
    if ( _headers.count("transfer-encoding"))
    {
        if (_headers["transfer-encoding"].find_first_of("chunked") != std::string::npos)
            _chunkedFlag = true;
        _bodyFlag = true;
    }
    if (_headers.count("host"))
    {
        size_t pos = _headers["host"].find_first_of(':');
        _serverName = _headers["host"].substr(0, pos);
    }
    if (_headers.count("content-type") && _headers["content-type"].find("multipart/form-data") != std::string::npos)
    {
        size_t pos = _headers["content-type"].find("boundary=", 0);
        if (pos != std::string::npos)
            this->_boundary = _headers["content-type"].substr(pos + 9, _headers["content-type"].size());
        this->_multiformFlag = true;
    }
}

short     HTTP::Request::errorCode()
{
    return (this->_errorCode);
}

/* Reset object variables to recive new request */

void    HTTP::Request::clear()
{
    _path.clear();
    _errorCode = 0;
    _query.clear();
    _fragment.clear();
    _method = NONE;
    _methodIndex = 1;
    _state = Request_Line;
    _bodyLength = 0;
    _chunkLength = 0x0;
    _storage.clear();
    _bodyStr = "";
    _keyStorage.clear();
    _headers.clear();
    _serverName.clear();
    _body.clear();
    _boundary.clear();
    _fieldsDoneFlag = false;
    _bodyFlag = false;
    _bodyDoneFlag = false;
    _completeFlag = false;
    _chunkedFlag = false;
    _multiformFlag = false;
}

/* Checks the value of header "Connection". If keep-alive, don't close the connection. */

bool        HTTP::Request::keepAlive()
{
    if (_headers.count("connection"))
    {
        if (_headers["connection"].find("close", 0) != std::string::npos)
            return (false);
    }
    return (true);
}

void            HTTP::Request::cutReqBody(int bytes)
{
    _bodyStr = _bodyStr.substr(bytes);
}

//             REQUEST                 */


/* RESPONSE */

// Constructors
// HTTP::Response::Response() : _responseCode(), _version(HTTP::HTTP_1_1), _headers(), _body("") {};

// HTTP::Response::Response(StatusCode responseCode, Version version, const std::map<std::string, HTTP::Header>& headers, const std::string& body)
//     : _responseCode(responseCode), _version(version), _headers(headers), _body(body) {};

// // Copy assignment operator
// HTTP::Response& HTTP::Response::operator=(const HTTP::Response& other) {
//     if (this != &other) {
//         _responseCode = other._responseCode;
//         _version = other._version;
//         _headers = other._headers;
//         _body = other._body;
//     }
//     return *this;
// }

// Methods
// std::string HTTP::Response::serialize() const {
//     std::string response = versionToString(_version) + " " + statusCodeToString(_responseCode) + LINE_END;
//     for (std::map<std::string, HTTP::Header>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
//         response += it->second.serialize();
//     response += LINE_END;
//     response += _body;
//     return response;
// }

// HTTP::Response HTTP::Response::deserialize(const std::string &response) {
//     std::vector<std::string> segments = utils::split(response, std::string(LINE_END) + std::string(LINE_END));
//     if (segments.size() < 2)
//         throw std::runtime_error("Invalid HTTP response: " + response);
//     std::string headerSegment = segments[0];
//     segments.erase(segments.begin());
//     std::string body = utils::concat(segments, LINE_END);
//     std::vector<std::string> headerLines = utils::split(headerSegment, std::string(LINE_END));
//     const std::string& responseCodeLine = headerLines[0];
//     std::vector<std::string> responseCodeSegments = utils::split(responseCodeLine, " ");
//     Version version = stringToVersion(responseCodeSegments[0]);
//     int responseCode = utils::strToInt(responseCodeSegments[1]);
//     headerLines.erase(headerLines.begin());
//     std::map<std::string, Header> headers;
//     for (std::vector<std::string>::const_iterator it = headerLines.begin(); it != headerLines.end(); ++it) {
//         const Header header = Header::deserialize(*it);
//         headers.insert(std::make_pair(header.getKey(), header));
//     }
//     return Response(StatusCode(responseCode), version, headers, body);
// }

Mime HTTP::Response::_mime;

HTTP::Response::Response()
{
    _targetFile = "";
    _body.clear();
    _bodyLength = 0;
    _responseContent = "";
    _responseBody = "";
    _location = "";
    _code = 0;
    _cgi = 0;
    _cgiResponseLength = 0;
    _autoIndex = 0;
}

HTTP::Response::~Response() {}

HTTP::Response::Response(HTTP::Request &req) : _request(req)
{
    _targetFile = "";
    _body.clear();
    _bodyLength = 0;
    _responseContent = "";
    _responseBody = "";
    _location = "";
    _code = 0;
    _cgi = 0;
    _cgiResponseLength = 0;
    _autoIndex = 0;
}

void   HTTP::Response::contentType()
{
    _responseContent.append("Content-Type: ");
    if(_targetFile.rfind(".", std::string::npos) != std::string::npos && _code == 200)
        _responseContent.append(_mime.getMimeType(_targetFile.substr(_targetFile.rfind(".", std::string::npos))) );
    else
        _responseContent.append(_mime.getMimeType("default"));
    _responseContent.append("\r\n");
}

void   HTTP::Response::contentLength()
{
    std::stringstream ss;
    ss << _responseBody.length();
    _responseContent.append("Content-Length: ");
    _responseContent.append(ss.str());
    _responseContent.append("\r\n");
}

void   HTTP::Response::connection()
{
    if(_request.getHeader("connection") == "keep-alive")
        _responseContent.append("Connection: keep-alive\r\n");
}

void   HTTP::Response::server()
{
    _responseContent.append("Server: AMAnix\r\n");
}

void    HTTP::Response::location()
{
    if (_location.length())
        _responseContent.append("Location: "+ _location +"\r\n");
}

void    HTTP::Response::date()
{
    char date[1000];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S %Z", &tm);
    _responseContent.append("Date: ");
    _responseContent.append(date);
    _responseContent.append("\r\n");

}
// uri ecnoding
void    HTTP::Response::setHeaders()
{
    contentType();
    contentLength();
    connection();
    server();
    location();
    date();

    _responseContent.append("\r\n");
}

static bool fileExists (const std::string& f) {
    std::ifstream file(f.c_str());
    return (file.good());
}

static bool isDirectory(std::string path)
{
    struct stat file_stat;
    if (stat(path.c_str(), &file_stat) != 0)
        return (false);

    return (S_ISDIR(file_stat.st_mode));
}

static bool isAllowedMethod(HTTP::Method &method, Location &location, short &code)
{
    std::vector<short> methods = location.getMethods();
    if ((method == HTTP::GET && !methods[0]) || (method == HTTP::POST && !methods[1]) ||
       (method == HTTP::DELETE && !methods[2])|| (method == HTTP::PUT && !methods[3])||
        (method == HTTP::HEAD && !methods[4]))
    {
        code = 405;
        return (1);
    }
    return (0);
}

static bool    checkReturn(Location &loc, short &code, std::string &location)
{
    if (!loc.getReturn().empty())
    {
        code = 301;
        location = loc.getReturn();
        if (location[0] != '/')
            location.insert(location.begin(), '/');
        return (1);
    }
    return (0);
}

static std::string combinePaths(std::string p1, std::string p2, std::string p3)
{
    std::string res;
    int         len1;
    int         len2;

    len1 = p1.length();
    len2 = p2.length();
    if (p1[len1 - 1] == '/' && (!p2.empty() && p2[0] == '/') )
        p2.erase(0, 1);
    if (p1[len1 - 1] != '/' && (!p2.empty() && p2[0] != '/'))
        p1.insert(p1.end(), '/');
    if (p2[len2 - 1] == '/' && (!p3.empty() && p3[0] == '/') )
        p3.erase(0, 1);
    if (p2[len2 - 1] != '/' && (!p3.empty() && p3[0] != '/'))
        p2.insert(p1.end(), '/');
    res = p1 + p2 + p3;
    return (res);
}

static void replaceAlias(Location &location, HTTP::Request &request, std::string &targetFile)
{
    targetFile = combinePaths(location.getAlias(), request.getPath().substr(location.getPath().length()), "");
}

static void appendRoot(Location &location, HTTP::Request &request, std::string &targetFile)
{
    targetFile = combinePaths(location.getRootLocation(), request.getPath(), "");
}

int HTTP::Response::handleCgiTemp(std::string &location_key)
{
    std::string path;
    path = _targetFile;
    _cgiObj.clear();
    _cgiObj.setCgiPath(path);
    _cgi = 1;
    if (pipe(_cgiFd) < 0)
    {
        _code = 500;
        return (1);
    }
    _cgiObj.initEnvCgi(_request, _server.getLocationKey(location_key)); // + URI
    _cgiObj.execute(this->_code);
    return (0);
}

/* check a file for CGI (the extension is supported, the file exists and is executable) and run the CGI */
int        HTTP::Response::handleCgi(std::string &location_key)
{
    std::string path;
    std::string exten;
    size_t      pos;

    path = this->_request.getPath();
    if (path[0] && path[0] == '/')
        path.erase(0, 1);
    if (path == "cgi-bin")
        path += "/" + _server.getLocationKey(location_key)->getIndexLocation();
    else if (path == "cgi-bin/")
        path.append(_server.getLocationKey(location_key)->getIndexLocation());

    pos = path.find(".");
    if (pos == std::string::npos)
    {
        _code = 501;
        return (1);
    }
    exten = path.substr(pos);
    if (exten != ".py" && exten != ".sh")
    {
        _code = 501;
        return (1);
    }
    if (ConfigFile::checkFileExistence(path) != 1)
    {
        _code = 404;
        return (1);
    }
    if (ConfigFile::checkFilePermissons(path, 1) == -1 || ConfigFile::checkFilePermissons(path, 3) == -1)
    {
        _code = 403;
        return (1);
    }

    // Get the location iterator
    std::vector<Location>::iterator locationItr = _server.getLocationKey(location_key);
    if (locationItr == _server.getLocations().end()) {
        _code = 404; // Not Found
        return (1);
    }

    if (isAllowedMethod(_request.getMethod(), *locationItr, _code))
        return (1);
    _cgiObj.clear();
    _cgiObj.setCgiPath(path);
    _cgi = 1;
    if (pipe(_cgiFd) < 0)
    {
        _code = 500;
        return (1);
    }
    _cgiObj.initEnv(_request, locationItr); // + URI
    _cgiObj.execute(this->_code);
    return (0);
}

/*
    Compares URI with locations from config file and tries to find the best match.
    If match found, then location_key is set to that location, otherwise location_key will be an empty string.
*/

static void    getLocationMatch(std::string &path, std::vector<Location> locations, std::string &location_key)
{
    size_t biggest_match = 0;

    for(std::vector<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it)
    {
        if(path.find(it->getPath()) == 0)
        {
               if( it->getPath() == "/" || path.length() == it->getPath().length() || path[it->getPath().length()] == '/')
               {
                    if(it->getPath().length() > biggest_match)
                    {
                        biggest_match = it->getPath().length();
                        location_key = it->getPath();
                    }
               }
        }
    }
}
int    HTTP::Response::handleTarget()
{
    std::string location_key;
    getLocationMatch(_request.getPath(), _server.getLocations(), location_key);
    if (location_key.length() > 0)
    {
        Location target_location = *_server.getLocationKey(location_key);

        if (isAllowedMethod(_request.getMethod(), target_location, _code))
        {
            std::cout << "METHOD NOT ALLOWED \n";
            return (1);
        }
        if (_request.getBody().length() > target_location.getMaxBodySize())
        {
            _code = 413;
            return (1);
        }
        if (checkReturn(target_location, _code, _location))
            return (1);

		if (target_location.getPath().find("cgi-bin") != std::string::npos)
		{
            return (handleCgi(location_key));
		}

        if (!target_location.getAlias().empty())
        {
            replaceAlias(target_location, _request, _targetFile);
        }
        else
            appendRoot(target_location, _request, _targetFile);

        if (!target_location.getCgiExtension().empty())
        {

            if (_targetFile.rfind(target_location.getCgiExtension()[0]) != std::string::npos)
            {
                return (handleCgiTemp(location_key));
            }

        }
        if (isDirectory(_targetFile))
        {
            if (_targetFile[_targetFile.length() - 1] != '/')
            {
                _code = 301;
                _location = _request.getPath() + "/";
                return (1);
            }
            if (!target_location.getIndexLocation().empty())
                _targetFile += target_location.getIndexLocation();
            else
                _targetFile += _server.getIndex();
            if (!fileExists(_targetFile))
            {
                if (target_location.getAutoindex())
                {
                    _targetFile.erase(_targetFile.find_last_of('/') + 1);
                    _autoIndex = true;
                    return (0);
                }
                else
                {
                    _code = 403;
                    return (1);
                }
            }
            if (isDirectory(_targetFile))
            {
                _code = 301;
                if (!target_location.getIndexLocation().empty())
                    _location = combinePaths(_request.getPath(), target_location.getIndexLocation(), "");
                else
                    _location = combinePaths(_request.getPath(), _server.getIndex(), "");
                if (_location[_location.length() - 1] != '/')
                    _location.insert(_location.end(), '/');

                return (1);
            }
        }
    }
    else
    {
        _targetFile = combinePaths(_server.getRoot(), _request.getPath(), "");
        if (isDirectory(_targetFile))
        {
            if (_targetFile[_targetFile.length() - 1] != '/')
            {
                _code = 301;
                _location = _request.getPath() + "/";
                return (1);
            }
            _targetFile += _server.getIndex();
            if (!fileExists(_targetFile))
            {
                _code = 403;
                return (1);
            }
            if (isDirectory(_targetFile))
            {
                _code = 301;
                _location = combinePaths(_request.getPath(), _server.getIndex(), "");
                if(_location[_location.length() - 1] != '/')
                    _location.insert(_location.end(), '/');
                return (1);
            }
        }
    }
    return (0);
}

bool HTTP::Response::reqError()
{
    if(_request.errorCode())
    {
        _code = _request.errorCode();
        return (1);
    }
    return (0);
}

void HTTP::Response::setServerDefaultErrorPages()
{
    _responseBody = utils::getErrorPage(_code);
}

void HTTP::Response::buildErrorBody()
{
        if( !_server.getErrorPages().count(_code) || _server.getErrorPages().at(_code).empty() ||
         _request.getMethod() == DELETE || _request.getMethod() == POST)
        {
            setServerDefaultErrorPages();
        }
        else
        {
            if(_code >= 400 && _code < 500)
            {
                _location = _server.getErrorPages().at(_code);
                if(_location[0] != '/')
                    _location.insert(_location.begin(), '/');
                _code = 302;
            }

            _targetFile = _server.getRoot() +_server.getErrorPages().at(_code);
            short old_code = _code;
            if(readFile())
            {
                _code = old_code;
                _responseBody = utils::getErrorPage(_code);
            }
        }

}
void    HTTP::Response::buildResponse()
{
    if (reqError() || buildBody())
        buildErrorBody();
    if (_cgi)
        return ;
    else if (_autoIndex)
    {
        std::cout << "AUTO index " << std::endl;
        if (utils::buildHtmlIndex(_targetFile, _body, _bodyLength))
        {
            _code = 500;
            buildErrorBody();
        }
        else
            _code = 200;
        _responseBody.insert(_responseBody.begin(), _body.begin(), _body.end());
    }
    setStatusLine();
    setHeaders();
    if (_request.getMethod() != HEAD && (_request.getMethod() == GET || _code != 200))
        _responseContent.append(_responseBody);
}

void HTTP::Response::setErrorResponse(short code)
{
    _responseContent = "";
    _code = code;
    _responseBody = "";
    buildErrorBody();
    setStatusLine();
    setHeaders();
    _responseContent.append(_responseBody);
}

/* Returns the entire reponse ( Headers + Body )*/
std::string HTTP::Response::getRes()
{
    return (_responseContent);
}

/* Returns the length of entire reponse ( Headers + Body) */
size_t HTTP::Response::getLen() const
{
	return (_responseContent.length());
}

/* Constructs Status line based on status code. */
void        HTTP::Response::setStatusLine()
{
    _responseContent.append("HTTP/1.1 " + utils::toString(_code) + " ");
    _responseContent.append(utils::statusCodeString(_code));
    _responseContent.append("\r\n");
}

int    HTTP::Response::buildBody()
{
    if (_request.getBody().length() > _server.getClientMaxBody())
    {
        _code = 413;
        return (1);
    }
    if ( handleTarget() )
        return (1);
    if (_cgi || _autoIndex)
        return (0);
    if (_code)
        return (0);
    if (_request.getMethod() == GET || _request.getMethod() == HEAD)
    {
        if (readFile())
            return (1);
    }
     else if (_request.getMethod() == POST || _request.getMethod() == PUT)
    {
        if (fileExists(_targetFile) && _request.getMethod() == POST)
        {
            _code = 204;
            return (0);
        }
        std::ofstream file(_targetFile.c_str(), std::ios::binary);
        if (file.fail())
        {
            _code = 404;
            return (1);
        }

        if (_request.getMultiformFlag())
        {
            std::string body = _request.getBody();
            body = removeBoundary(body, _request.getBoundary());
            file.write(body.c_str(), body.length());
        }
        else
        {
            file.write(_request.getBody().c_str(), _request.getBody().length());
        }
    }
    else if (_request.getMethod() == DELETE)
    {
        if (!fileExists(_targetFile))
        {
            _code = 404;
            return (1);
        }
        if (remove( _targetFile.c_str() ) != 0 )
        {
            _code = 500;
            return (1);
        }
    }
    _code = 200;
    return (0);
}

int HTTP::Response::readFile()
{
    std::ifstream file(_targetFile.c_str());

    if (file.fail())
    {
        _code = 404;
        return (1);
    }
    std::ostringstream ss;
	ss << file.rdbuf();
    _responseBody = ss.str();
    return (0);
}

void     HTTP::Response::setServer(ServerConfig &server)
{
    _server = server;
}

void    HTTP::Response::setRequest(HTTP::Request &req)
{
    _request = req;
}

void        HTTP::Response::cutRes(size_t i)
{
    _responseContent = _responseContent.substr(i);
}

void   HTTP::Response::clear()
{
    _targetFile.clear();
    _body.clear();
    _bodyLength = 0;
    _responseContent.clear();
    _responseBody.clear();
    _location.clear();
    _code = 0;
    _cgi = 0;
    _cgiResponseLength = 0;
    _autoIndex = 0;
}

int      HTTP::Response::getCode() const
{
    return (_code);
}

int    HTTP::Response::getCgiState()
{
    return (_cgi);
}

std::string HTTP::Response::removeBoundary(std::string &body, std::string &boundary)
{
    std::string buffer;
    std::string new_body;
    std::string filename;
    bool is_boundary = false;
    bool is_content = false;

    if (body.find("--" + boundary) != std::string::npos && body.find("--" + boundary + "--") != std::string::npos)
    {
        for (size_t i = 0; i < body.size(); i++)
        {
            buffer.clear();
            while(body[i] != '\n')
            {
                buffer += body[i];
                i++;
            }
            if (!buffer.compare(("--" + boundary + "--\r")))
            {
                is_content = true;
                is_boundary = false;
            }
            if (!buffer.compare(("--" + boundary + "\r")))
            {
                is_boundary = true;
            }
            if (is_boundary)
            {
                if (!buffer.compare(0, 31, "Content-Disposition: form-data;"))
                {
                    size_t start = buffer.find("filename=\"");
                    if (start != std::string::npos)
                    {
                        size_t end = buffer.find("\"", start + 10);
                        if (end != std::string::npos)
                            filename = buffer.substr(start + 10, end);
                    }
                }
                else if (!buffer.compare(0, 1, "\r") && !filename.empty())
                {
                    is_boundary = false;
                    is_content = true;
                }

            }
            else if (is_content)
            {
                if (!buffer.compare(("--" + boundary + "\r")))
                {
                    is_boundary = true;
                }
                else if (!buffer.compare(("--" + boundary + "--\r")))
                {
                    new_body.erase(new_body.end() - 1);
                    break ;
                }
                else
                    new_body += (buffer + "\n");
            }

        }
    }

    body.clear();
    return (new_body);
}

void      HTTP::Response::setCgiState(int state)
{
    _cgi = state;
}
//            RESPONSE                 */
