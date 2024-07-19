
# RequestParser And Response Builder

The `RequestParser` class is a parser for HTTP requests. It is designed to handle HTTP requests by parsing the request line, headers, and body according to the HTTP protocol. The class provides mechanisms to handle different types of HTTP methods, validate URIs, and manage headers and bodies.

## Classes
- [I. Request Parser](#requestparser)
- [II. Response Builder](#responsebuilder)
- [III. Mime Handling](#mimehandling)


## RequestParser

### Constructor & Destructor

- `Request::Request()`: Initializes the HTTP methods, request path, query, fragment, and other member variables to their default states.
- `Request::~Request()`: Destructor for the `Request` class. Cleans up resources if necessary.

### Member Functions

- `bool checkUriPos(std::string path)`: Validates if a URI path is valid, ensuring it does not traverse directories above the root.
- `bool allowedCharURI(uint8_t ch)`: Checks if a character is allowed in a URI according to the RFC standards.
- `bool isToken(uint8_t ch)`: Checks if a character is allowed in HTTP field names.
- `void trimStr(std::string &str)`: Trims leading and trailing whitespace from a string.
- `void toLower(std::string &str)`: Converts a string to lowercase.
- `void Request::feed(char *data, size_t size)`: Feeds data to the parser, processing it according to the current state.
- `bool Request::parsingCompleted()`: Checks if the request parsing is complete.
- `HttpMethod &Request::getMethod()`: Returns the HTTP method of the request.
- `std::string &Request::getPath()`: Returns the request path.
- `std::string &Request::getQuery()`: Returns the query string.
- `std::string &Request::getFragment()`: Returns the URI fragment.
- `std::string Request::getHeader(std::string const &name)`: Retrieves the value of a specified header.
- `const std::map<std::string, std::string> &Request::getHeaders() const`: Returns a map of all headers.
- `std::string Request::getMethodStr()`: Returns the HTTP method as a string.
- `std::string &Request::getBody()`: Returns the body of the request.
- `std::string Request::getServerName()`: Retrieves the server name from the "Host" header.
- `bool Request::getMultiformFlag()`: Checks if the request is of type multipart/form-data.
- `std::string &Request::getBoundary()`: Returns the boundary string for multipart/form-data.
- `void Request::setBody(std::string body)`: Sets the body of the request.
- `void Request::setMethod(HttpMethod & method)`: Sets the HTTP method for the request.
- `void Request::setHeader(std::string &name, std::string &value)`: Sets a header for the request, trimming and converting the name to lowercase.
- `void Request::setMaxBodySize(size_t size)`: Sets the maximum allowed body size.
- `void Request::printMessage()`: Prints the request details including method, path, headers, and body.
- `void Request::_handle_headers()`: Handles and processes request headers to determine if the body is required and if it's chunked.
- `short Request::errorCode()`: Returns the error code of the request parsing process.
- `void Request::clear()`: Resets the request object to its initial state, clearing all data.
- `bool Request::keepAlive()`: Checks the "Connection" header to determine if the connection should be kept alive.
- `void Request::cutReqBody(int bytes)`: Cuts the request body from the beginning by a specified number of bytes.

## HTTP Methods Supported

- GET
- POST
- DELETE
- PUT
- HEAD

## State Machine

The `Request` class uses a state machine to parse the HTTP request. The states include handling request lines, headers, and bodies, including chunked transfer encoding.

## Error Codes

- 400: Bad Request
- 501: Not Implemented
- 414: URI Too Long

## Usage Example

Here's a simple example of how you might use the `Request` class:

```cpp
#include "Request.hpp"

int main() {
    Request req;
    char data[] = "GET /index.html HTTP/1.1\r\nHost: example.com\r\n\r\n";
    req.feed(data, sizeof(data) - 1);

    if (req.parsingCompleted()) {
        std::cout << "Request Method: " << req.getMethodStr() << std::endl;
        std::cout << "Request Path: " << req.getPath() << std::endl;
        std::cout << "Host Header: " << req.getHeader("host") << std::endl;
    } else {
        std::cout << "Error Code: " << req.errorCode() << std::endl;
    }

    return 0;
}
```

## Notes

- Ensure to handle all edge cases and errors appropriately.
- The class handles chunked transfer encoding and multipart/form-data.

## ResponseBuilder

The `Response` class is responsible for handling HTTP responses in your web server. It manages constructing and sending HTTP responses, including setting headers, handling errors, and processing CGI scripts.

Here's a detailed breakdown of the class:

### Member Variables
- `_target_file`: The file path for the requested resource.
- `_body`: The body of the HTTP response.
- `_body_length`: The length of the response body.
- `_response_content`: The entire HTTP response content, including headers.
- `_response_body`: The actual content of the response body.
- `_location`: The location header for redirections.
- `_code`: The HTTP status code of the response.
- `_cgi`: Flag indicating if CGI handling is active.
- `_cgi_response_length`: The length of the CGI response.
- `_auto_index`: Flag indicating if auto-indexing is enabled.

### Constructor and Destructor
- `Response()`: Initializes member variables to default values.
- `Response(Request &req)`: Initializes member variables and assigns the incoming request.
- `~Response()`: Destructor that cleans up if necessary.

### Public Methods

#### Header Methods
- `contentType()`: Sets the Content-Type header based on the file extension.
- `contentLength()`: Sets the Content-Length header based on the length of the response body.
- `connection()`: Sets the Connection header based on the request header.
- `server()`: Sets the Server header with a static value "AMAnix".
- `location()`: Sets the Location header if a location string is present.
- `date()`: Sets the Date header with the current date and time.

#### Response Building
- `setHeaders()`: Calls header-setting methods and appends a newline to the response content.
- `buildErrorBody()`: Constructs the response body for error pages based on the status code.
- `buildResponse()`: Builds the full HTTP response, including status line, headers, and body.

#### Handling Requests
- `handleCgiTemp(std::string &location_key)`: Prepares for CGI execution by setting up pipes and environment variables.
- `handleCgi(std::string &location_key)`: Handles CGI execution by checking file type and permissions.
- `handleTarget()`: Determines the target file or directory and sets the response code and location based on various conditions.
- `reqError()`: Checks for request errors and sets the response code accordingly.

#### Utility Methods
- `setServer(ServerConf &server)`: Sets the server configuration for the response.
- `setRequest(Request &req)`: Sets the request for the response.
- `cutRes(size_t i)`: Trims the response content from index i.
- `clear()`: Clears all member variables to their default values.
- `getCode()`: Returns the HTTP status code.
- `getCgiState()`: Returns the CGI state.

#### File Handling
- `readFile()`: Reads the content of the target file into the response body.
- `removeBoundary(std::string &body, std::string &boundary)`: Removes multipart boundaries from the request body.

### Static and Helper Methods
- `fileExists(std::string &f)`: Checks if a file exists.
- `isDirectory(std::string path)`: Checks if a path is a directory.
- `isAllowedMethod(HttpMethod &method, Location &location, short &code)`: Checks if the HTTP method is allowed for a location.
- `checkReturn(Location &loc, short &code, std::string &location)`: Checks for redirection based on location settings.
- `combinePaths(std::string p1, std::string p2, std::string p3)`: Combines three path segments into a single path.
- `replaceAlias(Location &location, Request &request, std::string &target_file)`: Replaces path aliases with actual paths.
- `appendRoot(Location &location, Request &request, std::string &target_file)`: Appends the root location to the path.
- `getLocationMatch(std::string &path, std::vector<Location> locations, std::string &location_key)`: Finds the best matching location for a given path.

### Summary

The `Response` class is crucial for constructing and managing HTTP responses. It deals with setting HTTP headers, handling errors, processing CGI scripts, and managing file paths and permissions. It ensures that the server responds correctly to client requests by generating appropriate HTTP responses.

## MimeHandling

To properly serve different types of content, the RequestParser uses a `Mime` class that maps file extensions to their corresponding MIME types. This ensures that clients receive the correct content type in the HTTP response headers.

The `Mime` class is responsible for providing the MIME type for a given file extension. Here’s an overview of how it works:

### Mime.hpp

```cpp
#include <string>
#include <map>

class Mime {
public:
    Mime();
    std::string getMimeType(std::string extension);

private:
    std::map<std::string, std::string> _mime_types;
};
```

### Mime.cpp

```cpp
#include "Mime.hpp"

Mime::Mime()
{
    _mime_types[".html"] = "text/html";
    _mime_types[".htm"] = "text/html";
    _mime_types[".css"] = "text/css";
    _mime_types[".ico"] = "image/x-icon";
    _mime_types[".avi"] = "video/x-msvideo";
    _mime_types[".bmp"] = "image/bmp";
    _mime_types[".doc"] = "application/msword";
    _mime_types[".gif"] = "image/gif";
    _mime_types[".gz"] = "application/x-gzip";
    _mime_types[".jpg"] = "image/jpeg";
    _mime_types[".jpeg"] = "image/jpeg";
    _mime_types[".png"] = "image/png";
    _mime_types[".txt"] = "text/plain";
    _mime_types[".mp3"] = "audio/mp3";
    _mime_types[".pdf"] = "application/pdf";
    _mime_types["default"] = "text/html";
}

std::string Mime::getMimeType(std::string extension)
{
    if (_mime_types.count(extension))
        return (_mime_types[extension]);
    return (_mime_types["default"]);
}
```

## Integration with Response Class

In the `Response` class, the `Mime` object is used to set the `Content-Type` header based on the file extension of the requested resource. This ensures that the client correctly interprets the type of content being received.

### Example Usage

```cpp
#include "Mime.hpp"

Mime Response::_mime;

void Response::contentType()
{
    _response_content.append("Content-Type: ");
    if (_target_file.rfind(".", std::string::npos) != std::string::npos && _code == 200)
        _response_content.append(_mime.getMimeType(_target_file.substr(_target_file.rfind("."))));
    else
        _response_content.append(_mime.getMimeType("default"));
    _response_content.append("\r\n");
}
```

## How It Works

1. Initialization: The `Mime` class is initialized with a map of common file extensions to MIME types.

2. Determine Content Type: When a file is requested, the `Response::contentType()` method determines the MIME type based on the file extension. If the file extension is not recognized, it defaults to "text/html".

3. Set Header: The determined MIME type is then set in the `Content-Type` header of the HTTP response.

## Summary

The `Mime` class is an essential component of the `RequestParser`, ensuring that HTTP responses have the correct `Content-Type` headers based on the file extensions of requested resources. This improves client compatibility and ensures proper content rendering.