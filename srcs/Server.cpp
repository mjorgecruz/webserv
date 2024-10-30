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
#include <fstream>

Server::Server()
{}
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

void Server::setIndex(std::vector<std::string> index)
{
    _index = index;
}
void Server::setErrorPages(std::map<int, std::string> errorPages)
{
    _errorPages = errorPages;
}

void Server::addLocations(std::string path, Location *locations)
{
    (void) path;
    (void) locations;
}

void Server::serverChecker(std::string &line, std::ifstream &file)
{
    bool serverBracket = false;

    std::istringstream iss(line);
    std::string firstWord;
    iss >> firstWord;

    if  (firstWord != "server")
    {
        std::cout << "Server Error in ServerChecker\n";
        throw(std::exception());
        return ;
    }
    std::string remaining;
    std::getline(iss, remaining);

    remaining.erase(0, remaining.find_first_not_of(" \t"));
    
    if (remaining != "{")
    {
        while (std::getline(file, line))
        {
            line.erase(0, line.find_first_not_of(" \t"));
            if (line == "{")
            {
                serverBracket = true;
                break;
            }
            else if (!line.empty())
            {
                std::cout << "Expected: '{' , after 'server' \n";
                throw (std::exception());
            }
        }
    }
    else
    {
        serverBracket = true;
    }

    //while inside server brackets true iterates lines

    while(std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword;
        //keyword case switch or map to uuse find com functions e argumentos
    }

}

/*
void Server::serverKeywords(std::string key, std::string &line)
{
    //checks keyword parameters and atributes them

    //servername
    setHostname("");

    //index 
    setIndex("");

    //port division and set defaults if not both present
    setPorts(8080);
    setHost("localhost");

    //error_pages multiple ones allowed in the same line 
    setErrorPages(404,"");

    //locations
    addLocations("");
}
*/
