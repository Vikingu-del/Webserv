# Webserv

This project is about writing our own HTTP server. We will be able to test it with an actual browser. HTTP is one of the most used protocols on the internet.

## Creating a Webserver from Scratch in C++ 98: Notes and Bullet Points

### I. Introduction

1. This project aims to build a simple HTTP server from scratch in C++ 98.
2. The server should handle basic HTTP requests (GET, POST, HEAD) and serve static content (HTML, CSS, images).
3. Focus on understanding the core concepts of web server development and network programming.

### II. Requirements

1. **Language:** C++ 98
2. **Libraries:** Standard C++ library (iostream, string, vector, etc.), POSIX sockets (sys/socket.h, netinet/in.h, etc.)
3. **Functionality:**
   - Create a server socket and bind it to a specific port.
   - Listen for incoming client connections.
   - Accept client connections and handle them concurrently.
   - Parse HTTP requests from clients.
   - Serve static content based on the requested file path.
   - Respond with appropriate HTTP status codes and headers.
   - Handle errors gracefully.

### III. Implementation Steps

1. **Server Socket Creation and Binding:**
   - Create a socket: Use socket() system call to create a socket using the SOCK_STREAM type for TCP communication.
   - Bind the socket: Use bind() system call to associate the socket with a specific port and IP address (usually 0.0.0.0 for all interfaces).
2. **Listening for Connections:**
   - Start listening: Use listen() system call to put the socket in listening mode.
   - Accept incoming connections: Use accept() system call to accept a connection from a client. This returns a new socket for communication with the client.
3. **Handling Client Requests:**
   - Receive data: Use recv() system call to receive data from the client socket. This will contain the HTTP request.
   - Parse the HTTP request: Analyze the request line (method, URI, protocol version), headers, and body.
   - Extract the requested resource path: Use the URI to identify the file to be served.
   - Load the requested file: Open the file based on the extracted path and read its content.
4. **Sending Response:**
   - Compose HTTP response: Build the response header with the appropriate status code (200 OK for success, 404 Not Found for errors), content type, and other headers.
   - Send response: Use send() system call to send the response header and the file content to the client.
   - Close the connection: Close the client socket using close() system call.
5. **Concurrent Handling:**
   - Multithreading: Use threads to handle multiple client connections concurrently.
   - Multiprocessing: Alternatively, use multiple processes to handle connections.

### IV. Code Structure

1. **Main function:**
   - Initializes the server socket.
   - Listens for connections.
   - Creates threads or processes to handle connections.
2. **Handler function:**
   - Accepts the connection.
   - Receives the request.
   - Parses the request.
   - Serves the content.
   - Sends the response.
   - Closes the connection.
3. **Utility functions:**
   - Functions for parsing the HTTP request.
   - Functions for composing the HTTP response.
   - Functions for loading and serving files.

### V. Testing and Debugging

1. **Testing:** Use a web browser or curl to send HTTP requests to the server and verify its response.
2. **Debugging:** Use print statements, debuggers, or logging tools to identify and fix errors.

### VI. Project Notes

1. **Error Handling:** Implement robust error handling mechanisms for all system calls and file operations.
2. **Security:** Address potential security vulnerabilities such as buffer overflows and cross-site scripting.
3. **Concurrency:** Optimize for concurrent handling of multiple client connections.
4. **Scalability:** Design the server to handle increasing traffic and resource demands.

### VII. Additional Considerations

1. **Dynamic Content Generation:** Extend the server to handle dynamic content, such as server-side scripts or database queries.
2. **HTTPS Support:** Add SSL/TLS encryption for secure communication.
3. **Web Server Features:** Implement additional features like web server logs, authentication, authorization, and content caching.
4. **Documentation:** Document the code clearly with comments and a README file.

This outline provides a comprehensive roadmap for creating a web server from scratch in C++ 98. Remember to focus on the fundamental concepts of networking, HTTP protocols, and server development. Good luck with your project!



cgi 
Initialization: Set up pipes and fork the CGI process.
Sending Body: Write the request body to the CGI process and close the pipe.
Reading Response: Read the response from the CGI process, and handle the completion state.
Sending Client Response: Ensure that the connection is only closed once the response is fully sent and the CGI handler is done processing.