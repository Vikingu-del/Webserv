# CGI Common Gateway Interface
## Overview
This project implements a configuration parser for a web server. The configuration parser reads configuration files, parses them, and sets up the web server according to the specified settings. The main components of this parser are the FileConf, Location, ParseConf, ServerConf classes, and a key component, the Cgi class, which handles the execution of CGI scripts.

## Cgi Class
The Cgi class is responsible for managing and executing Common Gateway Interface (CGI) scripts. It sets up the necessary environment variables, handles input and output through pipes, and ensures that the CGI script is executed correctly.

### Key Responsibilities
- **Environment Setup:** Prepares the environment variables required for the CGI script execution.
- **Pipes for Communication:** Sets up pipes for input and output to facilitate communication between the web server and the CGI script.
- **Script Execution:** Uses the execve system call to execute the CGI script in a child process.
- **Resource Management:** Ensures proper cleanup of allocated resources to prevent memory leaks.

### Detailed Description of Key Methods
- **Constructors and Destructor:**
  - `Cgi()`: Initializes a new Cgi object with default values.
  - `Cgi(std::string path)`: Initializes a new Cgi object with the specified CGI script path.
  - `~Cgi()`: Cleans up allocated resources.
- **Initialization Methods:**
  - `initEnv(Request& req, const std::vector<Location>::iterator it_loc)`: Sets up the environment variables needed for CGI script execution.
  - `initEnvCgi(Request& req, const std::vector<Location>::iterator it_loc)`: Similar to initEnv, but tailored for a specific CGI setup.
- **Execution Methods:**
  - `execute(short &error_code)`: Executes the CGI script using execve.
- **Utility Methods:**
  - `setCgiPid(pid_t cgi_pid)`: Sets the process ID of the CGI script.
  - `setCgiPath(const std::string &cgi_path)`: Sets the path of the CGI script.
  - `clear()`: Resets the Cgi object to its initial state.
  - `findStart(const std::string path, const std::string delim)`: Finds the starting position of a delimiter in a path.
  - `decode(std::string &path)`: Decodes URL-encoded strings.
  - `getPathInfo(std::string& path, std::vector<std::string> extensions)`: Extracts the PATH_INFO from a URL.
- **Getters:**
  - `getEnv() const`: Returns the environment variables map.
  - `getCgiPid() const`: Returns the process ID of the CGI script.
  - `getCgiPath() const`: Returns the path of the CGI script.

### Example Usage
```cpp
Cgi cgi("/path/to/cgi/script");
Request req;
std::vector<Location>::iterator it_loc = // obtain the iterator to the relevant Location object
cgi.initEnv(req, it_loc);
short error_code = 0;
cgi.execute(error_code);

if (error_code != 0) {
    // Handle CGI execution error
}
