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
    _root = (std::getenv("HOME"));
    _root = _root + "/html";
    _host = "127.0.0.1";
    _autoIndex = -1;

}
Server::~Server()
{
    
}

void Server::setConfigs(std::string path)
{
    if (path.empty())
    {
        _host = "127.0.0.1";
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

std::vector< std::pair<std::string, Location *> > Server::getLocations()
{
    return _locations;   
}

std::vector<std::string> Server::getAllowedMethods()
{
    return _allowedMethods;
}

int Server::getMaxBodySize()
{
    return _maxBodySize;
}

int Server::getAutoIndex()
{
    return _autoIndex;
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
    _locations.push_back(make_pair(path, locations));
}

void Server::serverChecker(std::string &line, std::ifstream &file)
{
    bool serverBracket = false;
    std::istringstream iss(line);
    std::string firstWord;
    iss >> firstWord;

    if  (firstWord != "server")
        custtomServerThrow("Expected 'server' keyword at the beginning of the server block");
    std::string remaining;
    std::getline(iss, remaining);
    remaining.erase(0, remaining.find_first_not_of(" \t"));
    remaining.erase(remaining.find_last_not_of(" \t") + 1);

    if (remaining != "{")
    {
        while (std::getline(file, line))
        {
            if (line.find_first_not_of(" \t") == std::string::npos)
                continue;
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            if (line == "{")
            {
                serverBracket = true;
                break;
            }
            else if (!line.empty())
                custtomServerThrow("Expected '{' after 'server' keyword\n");
        }
    }
    else
    {
        serverBracket = true;
    }

    if (!serverBracket)
        custtomServerThrow("Missing opening '{' in block");
    while (std::getline(file, line))
    {
        if (line.find_first_not_of(" \t") == std::string::npos)
            continue;
          
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        if (line == "}")
            break;

        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword;

        if (keyword == "location")
        {
            Location *location = new Location();
            try
            {
                location->parseLocation(line, file);
                _locations.push_back(std::make_pair(location->getPath(), location));
            }
            catch (Location::exceptionAtLocation &e) 
            {
                delete location;
                custtomServerThrow("Location Creation Error @ ServerChecker");
            }
        }
        else
            serverKeywords(keyword, line);
    }
    this->setDefaultProperties();
}

void Server::serverKeywords(std::string key, std::string &line)
{
    line.erase(line.find_last_not_of(" \t") + 1);

    if (line[line.size() -1] != ';')
        custtomServerThrow("Expecting ';' .");
    line = line.substr(0, line.size() - 1);
    if (line.empty())
        custtomServerThrow("Expecting ';' .");
    if (key == "listen")
    {
        //would use all ports/ip configurations as long as no duplicate combination found. 
        //using last directive for us.
        keywordListen(line);
    }
    else if (key == "server_name")
    {
        //uses multiple in line or lines - ok
        keywordServerName(line);
    }
    else if (key == "index")
    {
        //only uses last directive - ok
        keywordIndex(line);
    }
    else if (key == "root")
    {
        //uses last directive - ok
        keywordRoot(line);
    }
    else if (key == "error_page")
    {
        //adding multiple - ok
        keywordErrorPages(line);
    }
    else if (key == "client_max_body_size")
    {
        // using last directive - ok
        keywordMaxBodySize(line);
    }
    else if (key == "allow_methods")
    {
        // using last directive - ok
        keyAllowMethods(line);
    }
    else
        custtomServerThrow("Invalid Keyword");
}

void Server::setRoot(std::string root)
{
    _root = root;
}

void Server::addAllowedMethods(std::string method)
{
    _allowedMethods.push_back(method);
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

void Server::setAutoIndex(int autoindex)
{
    _autoIndex = autoindex;
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

void Server::keywordListen(std::string &line)
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
            custtomServerThrow("Invalid IP: " + host);
        setHost(host);
        char* end;
        errno = 0;
        long portValue = std::strtol(portStr.c_str(), &end, 10);

        if (*end != '\0' || errno == ERANGE || portValue <= 0 || portValue > 65535)
            custtomServerThrow("Invalid Port: " + portStr);
        port = static_cast<int>(portValue);
    }
    else
    {
        char* end;
        errno = 0;
        long portValue = std::strtol(address.c_str(), &end, 10);
        if (*end != '\0' || errno == ERANGE || portValue <= 0 || portValue > 65535)
                    custtomServerThrow("Invalid Port: " + address );
        port = static_cast<int>(portValue);
    }
    std::string extraStuffinLine;
    iss >> extraStuffinLine;

    if(!extraStuffinLine.empty())
        custtomServerThrow("Listen.");
    setPorts(port);
}

void Server::keywordServerName(std::string &line)
{
    std::vector<std::string> hostnames = _hostname;
    std::string hostname;
    std::string temp;
    std::istringstream iss(line);
    iss >> temp;
    iss >> hostname;
    if (hostname.empty())
        custtomServerThrow("server_name unused.");
    if (hostname.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-.") != std::string::npos || hostname[0] == '-' || hostname[hostname.size() - 1] == '-')
        custtomServerThrow("Invalid server_name: " + hostname);
    hostnames.push_back(hostname);

    while (iss >> hostname)
    {
        if (hostname.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-.") != std::string::npos || hostname[0] == '-' || hostname[hostname.size() - 1] == '-')
            custtomServerThrow("Invalid server_name: " + hostname);
        hostnames.push_back(hostname);
    }
    setHostname(hostnames);
}

void Server::keywordIndex(std::string &line)
{
    std::vector<std::string> index = _index;
    std::string indexName, temp;
    std::istringstream iss(line);
    iss >> temp;
    iss >> indexName;

    if(indexName.empty())
        custtomServerThrow("Server Block: Index empty");

    index.clear();
    
    if (indexName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.") != std::string::npos)
            custtomServerThrow("Index format");
    index.push_back(indexName);
    while (iss >> indexName)
    {
        if (indexName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.") != std::string::npos)
            custtomServerThrow("Index format");
        index.push_back(indexName);
    }
    setIndex(index);
}

void Server::keywordRoot(std::string &line) 
{
    std::istringstream iss(line);
    std::string temp;
    std::string root;
    std::string keyword;
    iss >> keyword;
    iss >> root;
    iss >> temp;
    if (!temp.empty())
        custtomServerThrow("root invalid");
    if (root.empty())
        custtomServerThrow("root invalid");
    if (root[0] != '/')
        custtomServerThrow("root invalid");
    setRoot(root);
}

void Server::keywordErrorPages(std::string &line)
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
            if (isValidError(errorCode))
                errorCodes.push_back(errorCode);
            else
                custtomServerThrow("Invalid error code.");
        }
        else
        {
            errorPage = token;
            break;
        }
    }
    if (errorPage.empty() || errorPage.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/.") != std::string::npos)
        custtomServerThrow("Invalid error page path.");
    if (errorCodes.empty())
        custtomServerThrow("Invalid error page.");
    for (std::vector<int>::iterator it = errorCodes.begin(); it != errorCodes.end(); ++it)
    {
        errorPages[*it] = errorPage;
    }
    setErrorPages(errorPages);
}

void Server::keywordMaxBodySize(std::string &line)
{
    std::string sizeValue, temp, extra;
    std::istringstream iss(line);
    iss >> temp >> sizeValue;
    char* end;
    errno = 0;
    long maxBodySize = std::strtol(sizeValue.c_str(), &end, 10);
    if (errno == ERANGE || maxBodySize < 0)
        custtomServerThrow("Invalid max_body_size (overflow or negative value)");
    if (*end == 'M' && *(end + 1) == '\0')
    {
        maxBodySize *= 1024 * 1024;
    }
    else if (*end != '\0')
        custtomServerThrow("Invalid max_body_size");

    if (maxBodySize <= 0)
        custtomServerThrow("Invalid max_body_size (overflow or negative value)");
    iss >> extra;
    if(!extra.empty())
        custtomServerThrow("max_body_size");
    _maxBodySize = maxBodySize;
}

void Server::keyAllowMethods(std::string &line)
{
    std::istringstream iss(line);
    std::string temp;
    std::string method;
    iss >> temp;
    int i = 0;

    _allowedMethods.clear();
    while (iss >> method)
    {
        if (method != "POST" && method != "GET" && method != "DELETE")
            custtomServerThrow("allow_methods");
        addAllowedMethods(method);
        i++;
    }
    if  (i > 3 || i == 0)
        custtomServerThrow("allow_methods");
}

const char *Server::exceptionAtServer::what(void) const throw()
{
    return ("Error: At Parsing Server");
};