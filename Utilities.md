# Utilities

This guide provides an overview of the utility functions and classes available in the `/home/eseferi/Desktop/git_projects/Webserv/Utilities.md` directory. These utilities are designed to support the core functionalities of the web server, including logging mechanisms and various helper functions.

## Contents

1. [Logger Class](#logger-class)
2. [Utility Functions](#utility-functions)

## Logger Class

The `Logger` class provides a mechanism for logging messages with different priorities to either the console or a file. It is crucial for tracking server activities and debugging issues.

### Features

- Log Priorities: Supports different levels of log priorities, including DEBUG, INFO, and ERROR.
- Output Modes: Can log messages to a file or the console.
- Timestamping: Logs messages with timestamps for better traceability.

### Key Methods

- `logMsg(const char *color, Mode m, const char* msg, ...)`: Logs a message with the specified color and mode (console or file).
- `setPrio(LogPrio p)`: Sets the logging priority level.
- `setFileName(std::string name)`: Sets the filename for log file output.
- `setState(L_State s)`: Enables or disables logging.

### Usage Example

```cpp
Logger::setPrio(INFO);
Logger::setFileName("server.log");
Logger::logMsg(CYAN, FILE_OUTPUT, "Server started successfully.");
```

## Utility Functions

The `Utilities` directory also contains various helper functions that facilitate different tasks within the web server.

### `ft_stoi`

Converts a string to an integer. Ensures that the string contains only digits and is not too long.

- Parameters: `std::string str` - The string to convert.
- Returns: `int` - The integer value of the string.

### `fromHexToDec`

Converts a hexadecimal string to its decimal equivalent.

- Parameters: `const std::string& nb` - The hexadecimal string.
- Returns: `unsigned int` - The decimal value.

### `statusCodeString`

Maps HTTP status codes to their respective textual descriptions. Useful for generating informative responses.

- Parameters: `short statusCode` - The HTTP status code.
- Returns: `std::string` - The description of the status code.

### `getErrorPage`

Generates an HTML error page based on the HTTP status code. Useful for web servers to provide meaningful error messages to users.

- Parameters: `short statusCode` - The HTTP status code.
- Returns: `std::string` - The HTML content for the error page.

### `buildHtmlIndex`

Creates an HTML page listing the contents of a directory. Useful for generating directory indexes in web applications.

- Parameters:
    - `std::string &dir_name` - The directory to list.
    - `std::vector<uint8_t> &body` - The vector to store the HTML content.
    - `size_t &body_len` - The length of the HTML content.
- Returns: `int` - Status code indicating success or failure.
