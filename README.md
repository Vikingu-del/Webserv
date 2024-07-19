# 42 Webserv

Welcome to Webserv! This project involves creating a custom HTTP server from scratch in c++ that can be tested with an actual browser. As HTTP is a fundamental protocol on the internet, this project provides a hands-on approach to understanding and implementing server functionalities.

## Project Overview
This project is organized into five distinct directories, each handling a specific aspect of the HTTP server. Below is a brief overview of each component:

- [Requirements](./Requirements.md)
- [Parsing Configuration Files](./ConfigurationParser.md)
- [Parsing Requests And Building Responses](./RequestParser.md)
- [Server Manager](./ServerManager.md)
- [CGI](./Cgi.md)
- [Utilites](./Utilities.md)



## Installation

To get started with Webserv, follow these installation steps:

## 1. Clone the Repository

First, clone the Webserv repository to your local machine using Git:

   git clone https://github.com/Vikingu-del/Webserv.git

## 2. Build the Project
Im using Makefile for composition of my project so just do to the root directory and type:

   make

## 3. Run the Server
Start the server using the compiled executable and the configuration file you want where we are storing inside configs directory like below:

   ./webserv <config_file>

If you don't give a configuration it will take the default one I'm giving.


## License

Webserv is open-sourced under the MIT License. See the [LICENSE](./LICENSE) file for more details.

## Teammates

This project was brought to life with the invaluable contributions of the following team members:

- [Erik Seferi](https://github.com/Vikingu-del) - Project Lead
- [Ivan Petrunin](https://github.com/vanichx) - Parser of the Configuration File
- [Kyrylo Ilchenko](https://github.com/kilchenk) - ServerSocket Manager

We extend our deepest gratitude to everyone who contributed to the development and success of Webserv.