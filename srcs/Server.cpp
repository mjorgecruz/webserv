/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 15:11:21 by masoares          #+#    #+#             */
/*   Updated: 2024/10/29 15:23:12 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "ServerConfig.hpp"

ServerConfig::ServerConfig()
{}
ServerConfig::~ServerConfig()
{
    
}

void ServerConfig::setConfigs(std::string path)
{
    if (path.empty())
    {
        host = "10.11.4.4";
        errorPages[404] = "404.html";
        index.push_back("index.html");
        ports = 8080;
        hostname.push_back("10.11.4.4");

        
    }       
}


Location ServerConfig::operator[](int num) const
{
    if (num < (int) locations.size())
        return this->locations[num];
    else
        throw(std::exception());
}

size_t ServerConfig::locationsSize() const
{
    return locations.size();
}

std::string ServerConfig::getHost()
{
    return host;
}

int ServerConfig::getPorts()
{
    return ports;    
}

std::vector<std::string> ServerConfig::getHostname()
{
    return hostname;
}

std::vector<std::string> ServerConfig::getIndex()
{
    return index;
}
std::map<int, std::string> ServerConfig::getErrorPages()
{
    return errorPages;
}

std::vector<Location> ServerConfig::getLocations()
{
    return locations;   
}