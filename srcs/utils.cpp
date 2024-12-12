#include "general.hpp"

void handle_alarm(int sig) {
    (void) sig;
    std::cerr << "Timeout occurred while sending data" << std::endl;
    throw std::runtime_error("Timeout occurred");
}

/* Function has all the accepted error codes in HTTP 1.1 proctocol for this webserv project */
bool isValidError(int errorCode)
{
    int array[] = {200, 201, 202, 203, 204, 205, 206, 207, 208, 226, 300, 301, 302, 303, 304, 305, 306, 307, 308, 500, 501, 502, 503, 504, 505, 506, 507, 508, 510, 511, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417 ,418, 420, 421, 422, 423, 424, 425, 426, 428, 429, 431, 451};
    std::vector<int> valid;
    valid.assign(array, array + 60);

    for (size_t i = 0; i < valid.size(); i++)
    {
        if (valid[i] == errorCode)
            return true;
    }
    return false;
}

bool isNumeric(const std::string &str)
{
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (!std::isdigit(*it)) {
            return false;
        }
    }
    return true;
}

void custtomServerThrow(std::string str)
{
    std::cout << "\033[1;31mERROR -> Server: \033[0m";
    std::cout << str << std::endl;
    throw Server::exceptionAtServer();
}

void custtomLocationThrow(std::string str)
{
    std::cout << "\033[1;31mERROR: -> Location: \033[0m";
    std::cout << str << std::endl;
    throw Location::exceptionAtLocation();
}

void Server::printConfig() const
{
    std::cout << "\n\n------------------------Server BLOCK Config start" << std::endl;

    std::cout << "Host: " << _host << std::endl;
    std::cout << "Port: " << _ports << std::endl;
    std::cout << "Root: " << _root << std::endl;

    std::cout << "Server Names: ";
    for (std::vector<std::string>::const_iterator it = _hostname.begin(); it != _hostname.end(); ++it)
    {
        std::cout << *it;
        if (it + 1 != _hostname.end())
        {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;


    std::cout << "Index Files: ";
    for (std::vector<std::string>::const_iterator it = _index.begin(); it != _index.end(); ++it)
    {
        std::cout << *it;
        if (it + 1 != _index.end())
        {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;

    std::cout << "Error Pages:" << std::endl;
    for (std::map<int, std::string>::const_iterator it = _errorPages.begin(); it != _errorPages.end(); ++it)
    {
        std::cout << "  Error " << it->first << ": " << it->second << std::endl;
    }
    std::cout << "Max Body Size: " << _maxBodySize << std::endl;

    for (std::vector < std::pair <std::string, Location*> >::const_iterator loc = _locations.begin(); loc != _locations.end(); ++loc)
    {
        std::cout << "\n--------------------------------[LOCATION BLOCK]" << std::endl;
        std::cout << "Location Path: " << loc->first << std::endl;
        loc->second->printLocationConfig();
    }

    for (std::vector<std::string>::const_iterator it = _allowedMethods.begin(); it != _allowedMethods.end(); ++it)
    {
        std::cout << *it;
        if (it + 1 != _allowedMethods.end())
        {
            std::cout << ", ";
        }
    }

    std::cout << "\n---------------------------------END SERVER BLOCK\n\n" << std::endl;
}

/*Funtion to print all the configurations of each location class*/

void Location::printLocationConfig() const {
    std::cout << "  Root: " << _root << std::endl;
    std::cout << "  Index Files: ";
    for (std::vector<std::string>::const_iterator it = _index.begin(); it != _index.end(); ++it) {
        std::cout << *it;
        if (it + 1 != _index.end()) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
    std::cout << "  CGI Path: " << _cgiPath << std::endl;
    std::cout << "  Allowed Methods: ";
    for (std::vector<std::string>::const_iterator it = _allowedMethods.begin(); it != _allowedMethods.end(); ++it) {
        std::cout << *it;
        if (it + 1 != _allowedMethods.end()) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
    std::cout << "  Error Pages:" << std::endl;
    for (std::map<int, std::string>::const_iterator it = _errorPages.begin(); it != _errorPages.end(); ++it) {
        std::cout << "    Error " << it->first << ": " << it->second << std::endl;
    }
    std::cout << "  Redirect: " << _redirect << std::endl;
    std::cout << "  Auto Index: " << _autoIndex << std::endl;
}


void s_info::printInfoConfig() const
{
    std::cout << "---------- Printing T_INFO -------" << std::endl;
    if (!_host.empty())
        std::cout << "host: " << _host << std::endl;
    if (_ports)
        std::cout << "ports: " << _ports << std::endl;
    if (!_root.empty())
        std::cout << "root: " << _root << std::endl;
    if (!_hostname.empty())
    {
        std::cout << "Hostnames: ";
        for (size_t i = 0; i < _hostname.size(); ++i)
        {
            std::cout << " " << _hostname[i];
        }
        std::cout << ";" << std::endl;
    }

    if (!_index.empty())
    {
        std::cout << "index: ";
        for (size_t i = 0; i < _index.size(); ++i)
        {
            std::cout << " " << _index[i];
        }
        std::cout << ";" << std::endl;
    }
    std::cout << "autoindex: " << _autoIndex << std::endl;

    for (std::map<int, std::string>::const_iterator it = _errorPages.begin(); it != _errorPages.end(); ++it)
    {
        std::cout << "error_page: " << it->first << " " << it->second << std::endl;
    }

    if (!_allowedMethods.empty())
    {
        std::cout << "allow_methods: \n";
        for (size_t i = 0; i < _allowedMethods.size(); ++i)
        {
            std::cout << _allowedMethods[i] << std::endl;
        }
        std::cout << ";" << std::endl;
    }

    std::cout << "BodySize: " << _maxBodySize << std::endl;
    std::cout << "status: " << _status << std::endl;
    std::cout << "redirect: " << _redirect << std::endl;
    std::cout << "cgi_path: " << _cgiPath << std::endl;

    for (std::map<std::string, Location *>::const_iterator it = _locations.begin(); it != _locations.end(); ++it)
    {
        std::cout << "Locations paths: " << it->first << " " << it->second->getPath() << std::endl;
    }
}
