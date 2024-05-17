#include "Http.hpp"

void	testHttpHandler()
{
	std::string requestExample = "GET /index.html HTTP/1.1\r\n"
	"Host: www.example.com\r\n"
	"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:89.0) Gecko/20100101 Firefox/89.0\r\n"
	"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
	"Accept-Language: en-US,en;q=0.5\r\n"
	"Accept-Encoding: gzip, deflate, br\r\n"
	"Connection: keep-alive\r\n"
	"Upgrade-Insecure-Requests: 1\r\n"
	"\r\n";
	HTTP::Request requestParser = HTTP::Request::deserialize(requestExample);
	std::cout << "Example http request:\n\n" << requestExample << std::endl << "------------------------------" << std::endl;
	std::cout << RED << "METHOD: " << HTTP::methodToString(requestParser.getMethod()) << RESET << std::endl;
	std::cout << PURPLE << "RESOURCE: " << requestParser.getResource() << RESET << std::endl;
	int j = 0;
	for (std::map<std::string, HTTP::Header>::const_iterator it = requestParser.getHeaders().begin(); it != requestParser.getHeaders().end(); ++it)
	{
		++j;
		std::cout << CYAN <<  "Header " << j << ":     " << it->first << ": " << it->second.getValue() << RESET << std::endl;
	}
	std::cout << YELLOW << "VERSION: " << HTTP::versionToString(requestParser.getVersion()) << RESET << std::endl;
	std::cout << "------------------------------" << std::endl;

	std::string responseExample = "HTTP/1.0 200 OK\r\n"
	"Server: Netscape-Communications/1.1\r\n"
	"Date: Tuesday, 25-Nov-97 01:22:04 GMT\r\n"
	"Last-modified: Thursday, 20-Nov-97 10:44:53 GMT\r\n"
	"Content-length: 6372\r\n"
	"Content-type: text/html\r\n"
	"\r\n"
	"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\r\n"
	"<HTML>";
	std::cout << "Example http response:\n\n" << responseExample << std::endl << "------------------------------" << std::endl;
	HTTP::Response responseParser = HTTP::Response::deserialize(responseExample);
	std::cout << RED << "RESPONSE CODE: " << responseParser.getResponseCode() << RESET << std::endl;
	std::cout << PURPLE << "HTTP VERSION: " << responseParser.getVersion() << RESET << std::endl;
	int i = 0;
	for (std::map<std::string, HTTP::Header>::const_iterator it = responseParser.getHeaders().begin(); it != responseParser.getHeaders().end(); ++it)
	{
		++i;
		std::cout << CYAN <<  "Header " << i << ":     " << it->first << ": " << it->second.getValue() << RESET << std::endl;
	}
	std::cout << YELLOW << "BODY: " << responseParser.getBody() << RESET << std::endl;
}