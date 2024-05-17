#include "Http.hpp"

void	testHttpHandler()
{
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