# Config Parser for My Web Server

## Overview
This project implements a configuration parser for a web server. The configuration parser reads configuration files, parses them, and sets up the web server according to the specified settings. The main components of this parser are the FileConf, Location, ParseConf, and ServerConf classes.

## Classes
### FileConf
The FileConf class is responsible for handling file-related operations. It ensures that configuration files are properly read and validated. This class includes methods to check if a file exists, if it is readable, and to determine the type of path (whether it is a file or a directory). Additionally, it reads the contents of configuration files and provides the path and size of the file for further processing.

### Location
The Location class manages settings related to specific paths within the web server configuration. Each location corresponds to a different endpoint or directory on the server. This class holds information about the root directory for the location, whether directory listings (autoindex) are enabled, the index file, allowed HTTP methods (e.g., GET, POST), CGI settings, and other directives. It also provides methods to set and retrieve these properties, allowing fine-grained control over different parts of the web server.

### ParseConf
The ParseConf class is the core of the configuration parsing process. It reads the main configuration file, splits the content into different server blocks, removes comments and unnecessary whitespace, and processes each server configuration. This class is responsible for creating ServerConf objects for each server defined in the configuration file. It also includes error handling to ensure that the configuration file is valid and that all necessary directives are correctly specified.

### ServerConf
The ServerConf class represents the configuration for a single server. It includes settings such as the server name, listening port, root directory, index file, client body buffer size, error pages, and location blocks. The class provides methods to set and validate these properties, ensuring that the server configuration is complete and correct. It also includes functionality to initialize error pages, set up listening sockets, and manage the lifecycle of the server configuration.
### ServerConf
The ServerConf class represents the configuration for a single server. It includes settings such as the server name, listening port, root directory, index file, client body buffer size, error pages, and location blocks. The class provides methods to set and validate these properties, ensuring that the server configuration is complete and correct. It also includes functionality to initialize error pages, set up listening sockets, and manage the lifecycle of the server configuration.

### Example usage:
```python
server_conf = ServerConf()
server_conf.set_server_name("MyWebServer")
server_conf.set_listening_port(8080)
server_conf.set_root_directory("/var/www/html")
server_conf.set_index_file("index.html")
server_conf.set_client_body_buffer_size(1024)
server_conf.add_error_page(404, "/error.html")
server_conf.add_location("/api", "/var/www/api")
server_conf.add_location("/images", "/var/www/images", autoindex=True)
```

Please note that the above example is just a demonstration of how to use the ServerConf class. You may need to adapt it to your specific implementation and requirements.

For more details on the methods and properties of the ServerConf class, please refer to the class documentation.
