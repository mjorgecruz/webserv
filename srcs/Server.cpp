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
    if (std::getenv("HOME"))
    {
        _root = (std::getenv("HOME")); 
        _root += "/html";
    }
    _index.push_back("index.html");

    addErrorPage(204, _root + "204.html");
    addErrorPage(301, _root + "301.html");
    addErrorPage(403, _root + "403.html");
    addErrorPage(404, _root + "404.html");
    addErrorPage(409, _root + "409.html");
    addErrorPage(413, _root + "413.html");
    addErrorPage(500, _root + "500.html");
    addErrorPage(502, _root + "502.html");
    addErrorPage(503, _root + "503.html");
    addErrorPage(504, _root + "504.html");

    _maxBodySize = 1024 * 1024;


}
Server::~Server()
{
    
}

void Server::setConfigs(std::string path)
{
    if (path.empty())
    {
        
        _host = "127.0.0.1";
        _errorPages[404] = "404.html";
        _index.push_back("index.html");
        _ports = 8080;
        _hostname.push_back("10.11.4.4");
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
    (void) path;
    (void) locations;
}

void Server::addAllowedMethods(std::string method)
{
    _allowedMethods.push_back(method);
}

std::string Server::getRoot()
{
    return _root;
}

void Server::setRoot(std::string root)
{
    _root = root;
}