# Webserv

This repository contains the Webserv project, designed to build a fully functional HTTP server from scratch in C++. This project enhances the understanding of network programming, HTTP protocol handling, and concurrent programming, providing essential experience in web technologies and server management.

## Project Overview

### Objective

Implement a robust, scalable HTTP server capable of handling standard HTTP methods and managing client-server communications effectively.

### Key Concepts

- Network programming using sockets.
- HTTP/1.1 protocol compliance.
- Request parsing and response generation.
- MIME types and content management.
- Error handling and status codes.
- Concurrent programming and event-driven architecture (using select, poll, or epoll).

### Project Features

- Support for HTTP methods:
  - `GET`
  - `POST`
  - `DELETE`
- Configuration file parsing for server setup (ports, server names, error pages, etc.).
- Handling multiple client connections concurrently.
- Support for CGI (Common Gateway Interface) to execute external scripts in Python.
- Implementation of HTTP response status codes (200, 400, 404, 500, etc.).
- Static file serving and directory listing.

### Bonus Features

- **Cookies and Session Management:**
  - Implement HTTP cookies to maintain stateful sessions between clients and the server.
  - Provide examples demonstrating simple session creation, validation, and termination.
  - An account management system is included, allowing users to:
    - Log in securely.
    - Create new user accounts.
    - Delete existing accounts.

- **Multiple CGI Scripts Handling:**
  - Support concurrent execution of multiple CGI scripts to enhance dynamic content generation.
  - Efficiently manage CGI script lifecycles and resources, allowing simultaneous script execution without blocking the main server functionality.

### Technical Requirements

- Fully implemented in standard-compliant C++ (C++98).
- No external libraries (other than standard system libraries).
- Efficient memory and resource management.
- Clean, readable, maintainable, and modular code structure.

## How to Run

1. Clone the repository:

   ```bash
   git clone https://github.com/mjorgecruz/webserv.git
   cd webserv
   ```

2. Compile the project using provided Makefile:

   ```bash
   make
   ```

3. Run the server executable:

   ```bash
   ./webserv [configuration_file.conf]
   ```

If no configuration file is provided, the server will use default settings defined in the code.

4. Access the server via your web browser or tools like `curl`:

   ```bash
   curl http://localhost:8080
   ```

Replace `8080` with the port number configured in your server configuration file.


