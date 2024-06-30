/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 13:10:40 by eseferi           #+#    #+#             */
/*   Updated: 2024/06/30 01:07:08 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define LINE_END "\r\n"
#define MAX_CONTENT_LENGTH 3000000
#define TIMEOUT_PERIOD 5000

//! Parsing Definitions
#define CONNECTION_TIMEOUT 60
#define MAX_EVENTS 512
#define BUFF_SIZE 40000

//! checkFileExistence return values
#define THIS_IS_FILE 1
#define THIS_IS_DIR 2
#define THIS_IS_LINK 3
#define THIS_FILE_DOESNT_EXIST 4

//! checkFilePermissons return values
#define WRITE_PERMISSION 2
#define EXECUTE_PERMISSION 1
#define NO_PERMISSIONS "000"
#define THIS_FILE_HAS_PERMISSION 0
#define THIS_FILE_DOESNT_HAVE_PERMISSION -1
#define MAX_URI_LENGTH 8192 // this is the maximum length of a URI as NGINX defines it, longer URIs can cause security issues from DoS attacks

//! Color Definitions
#define RED   "\033[31m"
#define GREEN  "\033[32m"
#define YELLOW  "\033[33m"
#define PURPLE "\033[35m"
#define CYAN  "\033[36m"
#define RESET "\033[0m"
#define BLUE "\033[0;34m"
#define PINK "\033[0;35m"
#define BLD "\033[1m"
#define RST "\033[0m"
#define GST  4;

//! Error Error Status Definitions
#define STATUS_100 "Continue"
#define STATUS_101 "Switching Protocol"
#define STATUS_200 "OK"
#define STATUS_201 "Created"
#define STATUS_202 "Accepted"
#define STATUS_203 "Non-Authoritative Information"
#define STATUS_204 "No Content"
#define STATUS_205 "Reset Content"
#define STATUS_206 "Partial Content"
#define STATUS_300 "Multiple Choice"
#define STATUS_301 "Moved Permanently"
#define STATUS_302 "Moved Temporarily"
#define STATUS_303 "See Other"
#define STATUS_304 "Not Modified"
#define STATUS_307 "Temporary Redirect"
#define STATUS_308 "Permanent Redirect"
#define STATUS_400 "Bad Request"
#define STATUS_401 "Unauthorized"
#define STATUS_403 "Forbidden"
#define STATUS_404 "Not Found"
#define STATUS_405 "Method Not Allowed"
#define STATUS_406 "Not Acceptable"
#define STATUS_407 "Proxy Authentication Required"
#define STATUS_408 "Request Timeout"
#define STATUS_409 "Conflict"
#define STATUS_410 "Gone"
#define STATUS_411 "Length Required"
#define STATUS_412 "Precondition Failed"
#define STATUS_413 "Payload Too Large"
#define STATUS_414 "URI Too Long"
#define STATUS_415 "Unsupported Media Type"
#define STATUS_416 "Requested Range Not Satisfiable"
#define STATUS_417 "Expectation Failed"
#define STATUS_418 "I'm a teapot"
#define STATUS_421 "Misdirected Request"
#define STATUS_425 "Too Early"
#define STATUS_426 "Upgrade Required"
#define STATUS_428 "Precondition Required"
#define STATUS_429 "Too Many Requests"
#define STATUS_431 "Request Header Fields Too Large"
#define STATUS_451 "Unavailable for Legal Reasons"
#define STATUS_500 "Internal Server Error"
#define STATUS_501 "Not Implemented"
#define STATUS_502 "Bad Gateway"
#define STATUS_503 "Service Unavailable"
#define STATUS_504 "Gateway Timeout"
#define STATUS_505 "HTTP Version Not Supported"
#define STATUS_506 "Variant Also Negotiates"
#define STATUS_507 "Insufficient Storage"
#define STATUS_510 "Not Extended"
#define STATUS_511 "Network Authentication Required"
#define STATUS_UNDEFINED "Undefined"

enum	StatusCode {
    OK = 200,
    MOVED_PERMANENTLY = 301,
    MOVED_TEMPORARILY = 302,
    NOT_MODIFIED = 304,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503
};

enum ParsingState {
    Request_Line,
    Request_Line_Post_Put,
    Request_Line_Method,
    Request_Line_First_Space,
    Request_Line_URI_Path_Slash,
    Request_Line_URI_Path,
    Request_Line_URI_Query,
    Request_Line_URI_Fragment,
    Request_Line_Ver,
    Request_Line_HT,
    Request_Line_HTT,
    Request_Line_HTTP,
    Request_Line_HTTP_Slash,
    Request_Line_Major,
    Request_Line_Dot,
    Request_Line_Minor,
    Request_Line_CR,
    Request_Line_LF,
    Field_Name_Start,
    Fields_End,
    Field_Name,
    Field_Value,
    Field_Value_End,
    Chunked_Length_Begin,
    Chunked_Length,
    Chunked_Ignore,
    Chunked_Length_CR,
    Chunked_Length_LF,
    Chunked_Data,
    Chunked_Data_CR,
    Chunked_Data_LF,
    Chunked_End_CR,
    Chunked_End_LF,
    Message_Body,
    Parsing_Done
};
	
enum	Method {
    GET,
    POST,
    DELETE,
    PUT,
    HEAD,
    NONE
};

enum Version {
    HTTP_1_0,
    HTTP_1_1,
    HTTP_2_0
};
