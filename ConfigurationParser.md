# Configuration File Parsing Documentation

This document summarizes the work done on the configuration file parsing for the web server project, detailing the key components, the methods implemented, and their purposes. This will provide a clear understanding of the configuration parsing process and how it integrates with your part of the project.

## ConfigFileParser.hpp

The `ConfigFileParser` class is responsible for reading, parsing, and validating the configuration file. It breaks down the configuration file into manageable parts and initializes server configurations based on the provided directives.

### Attributes

- `_serversConfig`: A vector to store raw configuration data for each server.
- `_numOfServers`: The number of server configurations found in the file.
- `_servers`: A vector to store the parsed server configurations (ServerConfig objects).

### Constructor / Destructor

- `ConfigFileParser()`: Initializes the parser.
- `~ConfigFileParser()`: Destructor for cleanup.

### Main Methods

- `parseConfigFile(std::string &configFilePath)`: Reads and parses the configuration file located at the given path.
- `getServers() const`: Returns the parsed server configurations.

### Auxiliary Methods

- `removeComments(std::string &someString)`: Removes comments from the configuration content.
- `removeWhiteSpace(std::string &content)`: Removes unnecessary whitespace from the content.
- `findStartServer(size_t start, std::string &content)`: Finds the start of a server block in the configuration content.
- `findEndServer(size_t start, std::string &content)`: Finds the end of a server block.
- `findAndSplitServers(std::string &content)`: Splits the content into individual server configurations.
- `createServer(std::string &config, ServerConfig &server)`: Initializes a ServerConfig object from the given configuration string.

### Directive Handling Methods

- `handleListenDirective(ServerConfig &server, std::vector<std::string> &parameters, size_t &i)`: Processes the listen directive.
- `handleLocationDirective(ServerConfig &server, std::vector<std::string> &parameters, size_t &i, bool &flag_loc)`: Processes the location directive.
- `handleHostDirective(ServerConfig &server, std::vector<std::string> &parameters, size_t &i)`: Processes the host directive.
- `handleRootDirective(ServerConfig &server, std::vector<std::string> &parameters, size_t &i)`: Processes the root directive.
- `handleErrorPageDirective(std::vector<std::string> &parameters, size_t &i, std::vector<std::string> &error_codes)`: Processes the error_page directive.
- `handleClientMaxBodySizeDirective(ServerConfig &server, std::vector<std::string> &parameters, size_t &i, bool &flag_max_size)`: Processes the client_max_body_size directive.
- `handleServerNameDirective(ServerConfig &server, std::vector<std::string> &parameters, size_t &i)`: Processes the server_name directive.
- `handleIndexDirective(ServerConfig &server, std::vector<std::string> &parameters, size_t &i)`: Processes the index directive.
- `handleAutoindexDirective(ServerConfig &server, std::vector<std::string> &parameters, size_t &i, bool &flag_autoindex)`: Processes the autoindex directive.
- `handleUnsupportedDirective(bool flag_loc)`: Handles unsupported directives.

### Validation Methods

- `checkForDuplicateServers()`: Ensures there are no duplicate server configurations.
- `printServers()`: Prints the parsed server configurations for debugging purposes.

### Exception Handling

- `ParsingErrorException`: Custom exception class for handling parsing errors.

## ServerConfig.hpp

The `ServerConfig` class encapsulates the configuration for a single server. It includes various settings and methods to manipulate and validate these settings.

### Attributes

- `_port`: Port number the server listens on.
- `_host`: Host address of the server.
- `_server_name`: Server name for virtual hosting.
- `_root`: Root directory for the server.
- `_client_max_body_size`: Maximum size for client request bodies.
- `_index`: Default index file.
- `_autoindex`: Boolean flag for autoindexing.
- `_locations`: Vector of Location objects representing route configurations.
- `_listen_fd`: File descriptor for the listening socket.
- `_server_address`: Struct representing the server's address.
- `_error_pages`: Map of HTTP error codes to custom error pages.

### Getters

- `getPort()`: Returns the port number.
- `getHost()`: Returns the host address.
- `getServerName()`: Returns the server name.
- `getRoot()`: Returns the root directory.
- `getClientMaxBody()`: Returns the maximum client body size.
- `getIndex()`: Returns the index file.
- `getAutoindex()`: Returns the autoindex flag.
- `getErrorPages()`: Returns the error pages map.
- `getLocations()`: Returns the vector of Location objects.
- `getServerAddress()`: Returns the server address struct.
- `getListenFd()`: Returns the listening file descriptor.

### Setters

- `setServerName(std::string server_name)`: Sets the server name.
- `setHost(std::string parameter)`: Sets the host address.
- `setRoot(std::string root)`: Sets the root directory.
- `setFd(int fd)`: Sets the file descriptor.
- `setPort(std::string parameter)`: Sets the port number.
- `setClientMaxBodySize(std::string parameter)`: Sets the maximum client body size.
- `setErrorPages(const std::vector<std::string> &parameter)`: Sets the custom error pages.
- `setIndex(std::string index)`: Sets the index file.
- `setLocation(std::string nameLocation, std::vector<std::string> parameter)`: Adds a location configuration.
- `setAutoindex(std::string autoindex)`: Sets the autoindex flag.

### Methods

- `initErrorPages()`: Initializes the default error pages.
- `checkToken(std::string &parameter)`: Validates a configuration token.
- `isValidLocation(Location &location) const`: Validates a location configuration.
- `isValidHost(std::string host) const`: Validates the host address.
- `bindServer()`: Binds the server to the configured address and port.
- `checkLocations() const`: Validates all location configurations.
- `isValidErrorPages()`: Validates the custom error pages.

### Exception Handling

- `ServerConfigException`: Custom exception class for handling server configuration errors.

## How to Use This Information

- Socket Binding: Use the `getPort()` and `getHost()` methods to bind the server sockets.
- Request Handling: Use `getLocations()` to route incoming HTTP requests to the appropriate handlers.
- Error Handling: Use `getErrorPages()` to serve custom error pages.
- File Serving: Use `getRoot()` and `getIndex()` to locate and serve files.
- Method Restrictions: Ensure that only the allowed methods are processed for each route.
- CGI Handling: Use location configurations to handle CGI scripts.

This documentation should help you integrate the configuration parsing into your HTTP and socket handling code, ensuring that the server operates according to the specified configuration.
