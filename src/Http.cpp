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

void HTTP::Request::feed(const char *data, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        u_int8_t character = data[i];
        switch (_state)
        {
            case Request_Line:
                handleRequestLine(character);
                break;
            case Request_Line_Post_Put:
                handleRequestLinePostPut(character);
                break;
            case Request_Line_Method:
                handleRequestLineMethod(character);
                break;
            case Request_Line_First_Space:
                handleRequestLineFirstSpace(character);
                break;
            case Request_Line_URI_Path_Slash:
                handleRequestLineURIPathSlash(character);
                break;
            case Request_Line_URI_Path:
                handleRequestLineURIPath(character);
                break;
            case Request_Line_URI_Query:
                handleRequestLineURIQuery(character);
                break;
            case Request_Line_URI_Fragment:
                handleRequestLineURIFragment(character);
                break;
            case Request_Line_Ver:
                handleRequestLineVer(character);
                break;
            case Request_Line_HT:
                handleRequestLineHT(character);
                break;
            case Request_Line_HTT:
                handleRequestLineHTT(character);
                break;
            case Request_Line_HTTP:
                handleRequestLineHTTP(character);
                break;
            case Request_Line_HTTP_Slash:
                handleRequestLineHTTPSlash(character);
                break;
            case Request_Line_Major:
                handleRequestLineMajor(character);
                break;
            case Request_Line_Dot:
                handleRequestLineDot(character);
                break;
            case Request_Line_Minor:
                handleRequestLineMinor(character);
                break;
            case Request_Line_CR:
                handleRequestLineCR(character);
                break;
            case Request_Line_LF:
                handleRequestLineLF(character);
                break;
            case Field_Name_Start:
                handleFieldNameStart(character);
                break;
            case Fields_End:
                handleFieldsEnd(character);
                break;
            case Field_Name:
                handleFieldName(character);
                break;
            case Field_Value:
                handleFieldValue(character);
                break;
            case Field_Value_End:
                handleFieldValueEnd(character);
                break;
            case Chunked_Length_Begin:
                handleChunkedLengthBegin(character);
                break;
            case Chunked_Length:
                handleChunkedLength(character);
                break;
            case Chunked_Length_CR:
                handleChunkedLengthCR(character);
                break;
            case Chunked_Length_LF:
                handleChunkedLengthLF(character);
                break;
            case Chunked_Ignore:
                handleChunkedIgnore(character);
                break;
            case Chunked_Data:
                handleChunkedData(character);
                break;
            case Chunked_Data_CR:
                handleChunkedDataCR(character);
                break;
            case Chunked_Data_LF:
                handleChunkedDataLF(character);
                break;
            case Chunked_End_CR:
                handleChunkedEndCR(character);
                break;
            case Chunked_End_LF:
                handleChunkedEndLF(character);
                break;
            case Message_Body:
                handleMessageBody(character);
                break;
            case Parsing_Done:
                return;
        }
    }

    if (_state == Parsing_Done)
    {
        _bodyStr.append((char *)_body.data(), _body.size());
    }
}

void HTTP::Request::handleRequestLine(char character) {
    if (character == 'G')
        _method = GET;
    else if (character == 'P')
        _state = Request_Line_Post_Put;
    else if (character == 'D')
        _method = DELETE;
    else if (character == 'H')
        _method = HEAD;
    else
        setError(501, "Method Error Request_Line");

    _state = Request_Line_Method;
}

void HTTP::Request::handleRequestLinePostPut(char character)
{
    if (character == 'O')
        _method = POST;
    else if (character == 'U')
        _method = PUT;
    else
        setError(501, "Method Error Request_Line_Post_Put");

    _methodIndex++;
    _state = Request_Line_Method;
}

void HTTP::Request::handleRequestLineMethod(char character)
{
    if (character == _methodStr[_method][_methodIndex])
        _methodIndex++;
    else
        setError(501, "Method Error Request_Line_Method");

    if ((size_t)_methodIndex == _methodStr[_method].length())
        _state = Request_Line_First_Space;
}

void HTTP::Request::handleRequestLineFirstSpace(char character)
{
    if (character != ' ')
        setError(400, "Bad Character Request_Line_First_Space");

    _state = Request_Line_URI_Path_Slash;
}

void HTTP::Request::handleRequestLineURIPathSlash(char character)
{
    if (character == '/')
    {
        _state = Request_Line_URI_Path;
        _storage.clear();
    }
    else
        setError(400, "Bad Character Request_Line_URI_Path_Slash");
}

void HTTP::Request::handleRequestLineURIPath(char character)
{
    if (character == ' ')
    {
        _state = Request_Line_Ver;
        _path.append(_storage);
        _storage.clear();
    }
    else if (character == '?')
    {
        _state = Request_Line_URI_Query;
        _path.append(_storage);
        _storage.clear();
    }
    else if (character == '#')
    {
        _state = Request_Line_URI_Fragment;
        _path.append(_storage);
        _storage.clear();
    }
    else if (!allowedCharURI(character))
        setError(400, "Bad Character Request_Line_URI_Path");
    else if (_storage.size() > MAX_URI_LENGTH)
        setError(414, "URI Too Long Request_Line_URI_Path");

    _storage += character;
}

void HTTP::Request::handleRequestLineURIQuery(char character)
{
    if (character == ' ')
    {
        _state = Request_Line_Ver;
        _query.append(_storage);
        _storage.clear();
    }
    else if (character == '#')
    {
        _state = Request_Line_URI_Fragment;
        _query.append(_storage);
        _storage.clear();
    }
    else if (!allowedCharURI(character))
        setError(400, "Bad Character Request_Line_URI_Query");
    else if (_storage.size() > MAX_URI_LENGTH)
        setError(414, "URI Too Long Request_Line_URI_Query");

    _storage += character;
}

void HTTP::Request::handleRequestLineURIFragment(char character)
{
    if (character == ' ')
    {
        _state = Request_Line_Ver;
        _fragment.append(_storage);
        _storage.clear();
    }
    else if (!allowedCharURI(character))
        setError(400, "Bad Character Request_Line_URI_Fragment");
    else if (_storage.size() > MAX_URI_LENGTH)
        setError(414, "URI Too Long Request_Line_URI_Fragment");

    _storage += character;
}

void HTTP::Request::handleRequestLineVer(char character)
{
    if (checkUriPos(_path))
        setError(400, "Request URI ERROR: goes before root");

    if (character != 'H')
        setError(400, "Bad Character Request_Line_Ver");

    _state = Request_Line_HT;
}

void HTTP::Request::handleRequestLineHT(char character)
{
    if (character != 'T')
        setError(400, "Bad Character Request_Line_HT");

    _state = Request_Line_HTT;
}

void HTTP::Request::handleRequestLineHTT(char character)
{
    if (character != 'T')
        setError(400, "Bad Character Request_Line_HTT");

    _state = Request_Line_HTTP;
}

void HTTP::Request::handleRequestLineHTTP(char character)
{
    if (character != 'P')
        setError(400, "Bad Character Request_Line_HTTP");

    _state = Request_Line_HTTP_Slash;
}

void HTTP::Request::handleRequestLineHTTPSlash(char character)
{
    if (character != '/')
        setError(400, "Bad Character Request_Line_HTTP_Slash");

    _state = Request_Line_Major;
}

void HTTP::Request::handleRequestLineMajor(char character)
{
    if (!isdigit(character))
        setError(400, "Bad Character Request_Line_Major");

    _verMajor = character;
    _state = Request_Line_Dot;
}

void HTTP::Request::handleRequestLineDot(char character)
{
    if (character != '.')
        setError(400, "Bad Character Request_Line_Dot");

    _state = Request_Line_Minor;
}

void HTTP::Request::handleRequestLineMinor(char character)
{
    if (!isdigit(character))
        setError(400, "Bad Character Request_Line_Minor");

    _verMinor = character;
    _state = Request_Line_CR;
}

void HTTP::Request::handleRequestLineCR(char character)
{
    if (character != '\r')
        setError(400, "Bad Character Request_Line_CR");

    _state = Request_Line_LF;
}

void HTTP::Request::handleRequestLineLF(char character)
{
    if (character != '\n')
        setError(400, "Bad Character Request_Line_LF");

    _state = Field_Name_Start;
    _storage.clear();
}

void HTTP::Request::handleFieldNameStart(char character)
{
    if (character == '\r')
        _state = Fields_End;
    else if (isToken(character))
        _state = Field_Name;
    else
        setError(400, "Bad Character Field_Name_Start");
}

void HTTP::Request::handleFieldsEnd(char character)
{
    if (character == '\n')
    {
        _storage.clear();
        _fieldsDoneFlag = true;
        _handleHeaders();

        if (_bodyFlag)
            _state = (_chunkedFlag) ? Chunked_Length_Begin : Message_Body;
        else
            _state = Parsing_Done;
    }
    else
        setError(400, "Bad Character Fields_End");
}

void HTTP::Request::handleFieldName(char character)
{
    if (character == ':')
    {
        _keyStorage = _storage;
        _storage.clear();
        _state = Field_Value;
    }
    else if (!isToken(character))
        setError(400, "Bad Character Field_Name");

    _storage += character;
}

void HTTP::Request::handleFieldValue(char character)
{
    if (character == '\r')
    {
        setHeader(_keyStorage, _storage);
        _keyStorage.clear();
        _storage.clear();
        _state = Field_Value_End;
    }

    _storage += character;
}

void HTTP::Request::handleFieldValueEnd(char character)
{
    if (character == '\n')
        _state = Field_Name_Start;
    else
        setError(400, "Bad Character Field_Value_End");
}

void HTTP::Request::handleChunkedLengthBegin(char character)
{
    if (isxdigit(character) == 0)
        setError(400, "Bad Character Chunked_Length_Begin");

    std::stringstream s;
    s << character;
    s >> std::hex >> _chunkLength;
    _state = (_chunkLength == 0) ? Chunked_Length_CR : Chunked_Length;
}

void HTTP::Request::handleChunkedLength(char character)
{
    if (isxdigit(character) != 0)
    {
        int temp_len = 0;
        std::stringstream s;
        s << character;
        s >> std::hex >> temp_len;
        _chunkLength = _chunkLength * 16 + temp_len;
    }
    else if (character == '\r')
        _state = Chunked_Length_LF;
    else
        _state = Chunked_Ignore;
}

void HTTP::Request::handleChunkedLengthCR(char character)
{
    if (character != '\r')
        setError(400, "Bad Character Chunked_Length_CR");

    _state = Chunked_Length_LF;
}

void HTTP::Request::handleChunkedLengthLF(char character)
{
    if (character != '\n')
        setError(400, "Bad Character Chunked_Length_LF");

    _state = (_chunkLength == 0) ? Chunked_End_CR : Chunked_Data;
}

void HTTP::Request::handleChunkedIgnore(char character)
{
    if (character == '\r')
        _state = Chunked_Length_LF;
}

void HTTP::Request::handleChunkedData(char character)
{
    _body.push_back(character);
    if (--_chunkLength == 0)
        _state = Chunked_Data_CR;
}

void HTTP::Request::handleChunkedDataCR(char character)
{
    if (character != '\r')
        setError(400, "Bad Character Chunked_Data_CR");

    _state = Chunked_Data_LF;
}

void HTTP::Request::handleChunkedDataLF(char character)
{
    if (character != '\n')
        setError(400, "Bad Character Chunked_Data_LF");

    _state = Chunked_Length_Begin;
}

void HTTP::Request::handleChunkedEndCR(char character)
{
    if (character != '\r')
        setError(400, "Bad Character Chunked_End_CR");

    _state = Chunked_End_LF;
}

void HTTP::Request::handleChunkedEndLF(char character)
{
    if (character != '\n')
        setError(400, "Bad Character Chunked_End_LF");

    _bodyDoneFlag = true;
    _state = Parsing_Done;
}

void HTTP::Request::handleMessageBody(char character)
{
    _body.push_back(character);
    if (_body.size() >= _bodyLength)
        _bodyDoneFlag = true;
    _state = Parsing_Done;
}

void HTTP::Request::setError(int code, const std::string &message)
{
    _errorCode = code;
    _state = Parsing_Done;
    std::cerr << message << std::endl;
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

//             RESPONSE                */

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

int HTTP::Response::handleCgiTemp(Location &targetLocation)
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
    _cgiObj.initEnvCgi(_request, targetLocation); // + URI
    _cgiObj.execute(this->_code);
    return (0);
}

/* check a file for CGI (the extension is supported, the file exists and is executable) and run the CGI */
int        HTTP::Response::handleCgi(Location &targetLocation)
{
    std::string path;
    std::string exten;
    size_t      pos;

    path = this->_request.getPath();
    std::cout << "PATH: " << path << std::endl;
    if (path[0] && path[0] == '/')
        path.erase(0, 1);
    std::cout << "PATH after checking for slash: " << path << std::endl;
    if (path == "cgi-bin")
        path += "/" + targetLocation.getIndexLocation();
    else if (path == "cgi-bin/")
        path.append(targetLocation.getIndexLocation());

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
    if (isAllowedMethod(_request.getMethod(), targetLocation, _code))
        return (1);
    _cgiObj.clear();
    _cgiObj.setCgiPath(path);
    _cgi = 1;
    if (pipe(_cgiFd) < 0)
    {
        _code = 500;
        return (1);
    }
    _cgiObj.initEnv(_request, targetLocation); // + URI
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
        if( ("/" + path).find(it->getPath()) == 0)
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
    std::cout << "HANDLE TARGET" << std::endl;
    std::string location_key;
    getLocationMatch(_request.getPath(), _server.getLocations(), location_key);
    if (!location_key.empty())
    {
        std::pair<std::vector<Location>::iterator, bool> locPair = _server.getLocationKey(location_key);
        if (!locPair.second) {
            Logger::logMsg(RED, CONSOLE_OUTPUT,  "Location %s not found", location_key.c_str());
            _code = 404; // Or appropriate error code
            return (1);
        }
        std::cout << "Location found for key: " << location_key << std::endl;
        Location target_location = *locPair.first;
        if (isAllowedMethod(_request.getMethod(), target_location, _code))
        {
            std::cout << "METHOD NOT ALLOWED \n";
            return (1);
        }
        if (_request.getBody().length() > target_location.getMaxBodySize())
        {
            std::cout << "BODY TOO LARGE \n";
            _code = 413;
            return (1);
        }
        if (checkReturn(target_location, _code, _location)) {
            std::cout << "CHECK RETURN \n";
            return (1);
        }

		if (target_location.getPath().find("cgi-bin") != std::string::npos)
		{
            std::cout << "CGI BIN" << std::endl;
            return (handleCgi(target_location));
		}

        if (!target_location.getAlias().empty())
        {
            replaceAlias(target_location, _request, _targetFile);
        }
        else
            appendRoot(target_location, _request, _targetFile);

        if (!target_location.getCgiExtension().empty())
        {
            std::cout << "CGI EXTENSION" << std::endl;
            if (_targetFile.rfind(target_location.getCgiExtension()[0]) != std::string::npos)
            {
                return (handleCgiTemp(target_location));
            }

        }
        if (isDirectory(_targetFile))
        {
            std::cout << "IS DIRECTORY" << std::endl;
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
        std::cout << "NO LOCATION FOUND" << std::endl;
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
    if (reqError() || buildBody()) {
        std::cout << "ERROR" << std::endl;
        buildErrorBody();
    }
    if (_cgi) {
        std::cout << "it is Cgi" << std::endl;
        return ;
    }
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
    std::cout << "BUILD BODY" << std::endl;
    if (_request.getBody().length() > _server.getClientMaxBody())
    {
        std::cout << "BODY TOO LONG" << std::endl;
        _code = 413;
        return (1);
    }
    if (handleTarget())
        return (1);
    if (_cgi || _autoIndex) {
        std::cout << "CGI OR AUTOINDEX" << std::endl;
        return (0);
    }
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
