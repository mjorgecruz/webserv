/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 15:11:21 by masoares          #+#    #+#             */
/*   Updated: 2024/10/29 15:23:12 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Server.hpp"


Server::Server()
{
    _maxBodySize = 0;
    
    std::vector<std::string> possibleErrors;
    possibleErrors.push_back("204");
    possibleErrors.push_back("301");
    possibleErrors.push_back("403");
    possibleErrors.push_back("404");
    possibleErrors.push_back("409");
    possibleErrors.push_back("413");
    possibleErrors.push_back("500");
    possibleErrors.push_back("502");
    possibleErrors.push_back("503");
    possibleErrors.push_back("504");
    
    for (size_t i = 0; i < possibleErrors.size(); i++)
    {
        int error = static_cast<int> (strtol(possibleErrors[i].c_str(), NULL, 10));
        std::string root = (std::getenv("HOME"));
        root = root + "/html" + "/";
        addErrorPage(error, (root + possibleErrors[i] + ".html"));
    }

}
Server::~Server()
{
    
}

void Server::setConfigs(std::string path)
{
    if (path.empty())
    {
        
        _host = "127.0.0.1";
        _errorPages[404] = "/404.html";
        _index.push_back("index.html");
        _ports = 8080;
        _hostname.push_back("10.11.4.4");
        _allowedMethods.push_back("GET");
        _allowedMethods.push_back("POST");
        _allowedMethods.push_back("DELETE");

    }       
}


size_t Server::locationsSize() const
{
    return _locations.size();
}

std::string Server::getHost()
{
    return _host;
}

int Server::getPorts()
{
    return _ports;    
}

std::vector<std::string> Server::getHostname()
{
    return _hostname;
}

std::vector<std::string> Server::getIndex()
{
    return _index;
}
std::map<int, std::string> Server::getErrorPages()
{
    return _errorPages;
}

std::map<std::string, Location *> Server::getLocations()
{
    return _locations;   
}

std::vector<std::string> Server::getAllowedMethods()
{
    return _allowedMethods;
}


void Server::setHost(std::string host)
{
    _host = host;
}

void Server::setPorts(int port)
{
    _ports = port;
}
void Server::setHostname( std::vector<std::string> hostnames)
{
    _hostname = hostnames;
}

void Server::addIndex(std::string index)
{
    _index.push_back(index);
}
void Server::addErrorPage(int errorNum, std::string error)
{
    _errorPages[errorNum] = error;
}

void Server::addLocations(std::string path, Location *locations)
{
    _locations[path] = locations;
}

void Server::serverChecker(std::string &line, std::ifstream &file)
{
    bool serverBracket = false;
    std::istringstream iss(line);
    std::string firstWord;
    iss >> firstWord;

    if  (firstWord != "server")
    {
        std::cout << "Expected 'server' keyword at the beginning of the server block\n";
        throw(std::exception());
    }
    std::string remaining;
    std::getline(iss, remaining);
    remaining.erase(0, remaining.find_first_not_of(" \t"));

    if (remaining != "{")
    {
        while (std::getline(file, line))
        {
            if (line.find_first_not_of(" \t") == std::string::npos)
                continue;
            line.erase(0, line.find_first_not_of(" \t"));
            if (line == "{")
            {
                serverBracket = true;
                break;
            }
            else if (!line.empty())
            {
                std::cout << "Expected '{' after 'server' keyword\n";
                throw (std::exception());
            }
        }
    }
    else
    {
        serverBracket = true;
    }

    if (!serverBracket)
    {
        std::cout << "Missing opening '{' in block\n";
        throw (std::exception());
    }

    while (std::getline(file, line))
    {
        if (line.find_first_not_of(" \t") == std::string::npos)
            continue;
          
        line.erase(0, line.find_first_not_of(" \t"));
        if (line == "}")
        {
            break;
        }

        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword;

        if (keyword == "location")
        {
            Location *location = new Location();
            try
            {
                location->parseLocation(line, file);
                _locations[location->getPath()] = location;
                //addLocations(location->getPath(), location);
            }
            catch (std::exception &e)
            {
                delete location;
                std::cout << "Location Creation Error in ServerChecker @ WebservInitializer" << std::endl;
                throw(std::exception());
            }
        }
        else
        {
            serverKeywords(keyword, line);
        }
    }
    this->setDefaultProperties();
}

static bool isNumeric(const std::string &str)
{
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (!std::isdigit(*it)) {
            return false;
        }
    }
    return true;
}

void Server::serverKeywords(std::string key, std::string &line)
{
    line.erase(line.find_last_not_of(" \t") + 1);

    if (line[line.size() -1] != ';')
    {
        std::cout << "error: missing ';' for " << key << "\n";
        throw std::exception();
    }
    line = line.substr(0, line.size() - 1);

    if (line.empty())
    {
        std::cout << "error: empty value for " << key << "\n";
        throw std::exception();
    }

    if (key == "listen")
    {
        std::string address;
        std::string host;
        std::istringstream iss(line);
        iss >> address >> address;

        int port;
        size_t colonPos = address.find(':');

        if (colonPos != std::string::npos)
        {
            host = address.substr(0, colonPos);
            std::string portStr = address.substr(colonPos + 1);

            std::istringstream ipStream(host);
            std::string octet;
            int octetCount = 0;
            bool validIP = true;

            while (std::getline(ipStream, octet, '.'))
            {
                char* end;
                errno = 0;
                long octetValue = std::strtol(octet.c_str(), &end, 10);

                if (*end != '\0' || errno == ERANGE || octetValue < 0 || octetValue > 255)
                {
                    validIP = false;
                    break;
                }
                octetCount++;
            }
            if (octetCount != 4 || !validIP)
            {
                std::cout << "Invalid IP address format: " << host << "\n";
                throw std::exception();
            }
            setHost(host);
            char* end;
            errno = 0;
            long portValue = std::strtol(portStr.c_str(), &end, 10);
            if (*end != '\0' || errno == ERANGE || portValue <= 0 || portValue > 65535)
            {
                std::cout << "Invalid port number: " << portStr << "\n";
                throw std::exception();
            }
            port = static_cast<int>(portValue);
        }
        else
        {
            char* end;
            errno = 0;
            long portValue = std::strtol(address.c_str(), &end, 10);

            if (*end != '\0' || errno == ERANGE || portValue <= 0 || portValue > 65535)
            {
                std::cout << "Invalid port number: " << address << "\n";
                throw std::exception();
            }
            port = static_cast<int>(portValue);
        }
        setPorts(port);
    }
    else if (key == "server_name")
    {
        std::vector<std::string> hostnames = _hostname;
        std::string hostname;
        std::string temp;
        std::istringstream iss(line);
        iss >> temp;

        while (iss >> hostname)
        {
            if (hostname.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-.") != std::string::npos || hostname[0] == '-' || hostname[hostname.size() - 1] == '-')
            {
                std::cout << "Invalid server_name: " << hostname << "\n";
                throw std::exception();
            }
            hostnames.push_back(hostname);
        }
        setHostname(hostnames);
    }
    else if (key == "index")
    {
        std::vector<std::string> index = _index;
        std::string indexName;
        std::string temp;
        std::istringstream iss(line);
        iss >> temp;

        while (iss >> indexName)
        {
            if (indexName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.") != std::string::npos)
            {
                std::cout << "Invalid index format: " << indexName << "\n";
                throw std::exception();
            }
            index.push_back(indexName);
        }
        setIndex(index);
    }
    else if (key == "root")
    {
        std::istringstream iss(line);
        iss >> _root >> _root;
        std::cout << "root: " << _root << std::endl;
        if (_root[0] != '/')
        {
            std::cout << "Invalid root path: " << _root << "\n";
            throw std::exception();
        }
    }
    else if (key == "error_page")
    {
        std::map<int, std::string> errorPages = _errorPages;
        std::string token, temp;
        std::istringstream iss(line);
        iss >> temp;

        std::vector<int> errorCodes;
        std::string errorPage;

        while (iss >> token)
        {
            if (isNumeric(token))
            {
                int errorCode = std::atoi(token.c_str());
                if (errorCode < 400 || errorCode > 599)
                {
                    std::cout << "Invalid error code: " << errorCode << "\n";
                    throw std::exception();
                }
                errorCodes.push_back(errorCode);
            }
            else
            {
                errorPage = token;
                break;
            }
        }
        if (errorPage.empty() || errorPage.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/.") != std::string::npos)
        {
            std::cout << "Invalid error page path: " << errorPage << "\n";
            throw std::exception();
        }
        if (errorCodes.empty())
        {
            std::cout << "Invalid error: " << errorPage << "\n";
            throw std::exception();
        }
        for (std::vector<int>::iterator it = errorCodes.begin(); it != errorCodes.end(); ++it)
        {
            errorPages[*it] = errorPage;
        }
        setErrorPages(errorPages);
    }
    else if (key == "max_body_size")
    {
        std::string sizeValue, temp;
        std::istringstream iss(line);
        iss >> temp >> sizeValue;

        char* end;
        errno = 0;
        long maxBodySize = std::strtol(sizeValue.c_str(), &end, 10);

        if (errno == ERANGE || maxBodySize < 0)
        {
            std::cout << "Invalid max_body_size (overflow or negative value): " << sizeValue << "\n";
            throw std::exception();
        }
        if (*end == 'M' && *(end + 1) == '\0')
        {
            maxBodySize *= 1024 * 1024;
        }
        else if (*end != '\0') 
        {
            std::cout << "Invalid max_body_size: " << sizeValue << "\n";
            throw std::exception();
        }
        if (maxBodySize <= 0)
        {
            std::cout << "Invalid max_body_size (must be positive): " << sizeValue << "\n";
            throw std::exception();
        }
        _maxBodySize = maxBodySize;
    }
    else
    {
        std::cout << "Invalid keyword: " << key << "\n";
        throw std::exception();
    }
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

    for (std::map<std::string, Location*>::const_iterator loc = _locations.begin(); loc != _locations.end(); ++loc)
    {
        std::cout << "\n--------------------------------[LOCATION BLOCK]" << std::endl;
        std::cout << "Location Path: " << loc->first << std::endl;
        loc->second->printLocationConfig();
    }

    std::cout << "---------------------------------END SERVER BLOCK\n\n" << std::endl;
}


void Server::addAllowedMethods(std::string method)
{
    _allowedMethods.push_back(method);
}


void Server::setRoot(std::string root)
{
    _root = root;
}

std::string Server::getRoot()
{
    return _root;
}

void Server::setIndex(std::vector<std::string> index)
{
    _index = index;
}
void Server::setErrorPages(std::map<int, std::string> errorPages)
{
    _errorPages = errorPages;
}


void Server::setDefaultProperties( void )
{
    if (this->getAllowedMethods().empty())
    {
        addAllowedMethods("GET");
        addAllowedMethods("POST");
        addAllowedMethods("DELETE");
    }
    if (this->getRoot().empty())
    {
        if (std::getenv("HOME"))
        {
            _root = (std::getenv("HOME")); 
            _root += "/html";
        }
    }
    if (this->getIndex().empty())
        _index.push_back("index.html");

    if (this->_maxBodySize == 0)
        _maxBodySize = 1024 * 1024;


    for (std::map<int,std::string>::iterator it = _errorPages.begin(); it != _errorPages.end(); it++)
    {
        if ((it->second)[0] != '/')
        {
            addErrorPage(it->first, (_root + "/" + (it->second)));
        }
    }
}