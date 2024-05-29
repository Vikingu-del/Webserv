/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 13:10:40 by eseferi           #+#    #+#             */
/*   Updated: 2024/05/29 11:49:47 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define LINE_END "\r\n"
#define MAX_CONTENT_LENGTH 3000000
#define TIMEOUT_PERIOD 60.0

//! Parsing Definitions

//! checkFileExistence return values
#define THIS_IS_FILE 1
#define THIS_IS_DIR 2
#define THIS_IS_LINK 3
#define THIS_FILE_DOESNT_EXIST 4

//! checkFilePermissons return values
#define WRITE_PERMISSION 2
#define EXECUTE_PERMISSION 1
#define NO_PERMISSIONS 0
#define THIS_FILE_HAS_PERMISSION 0
#define THIS_FILE_DOESNT_HAVE_PERMISSION -1

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