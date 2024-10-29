/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 15:11:21 by masoares          #+#    #+#             */
/*   Updated: 2024/10/29 13:33:25 by masoares         ###   ########.fr       */
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
        t_config *conf = new t_config();
        conf->host = "10.11.4.4";
        conf->errorPage = "404.html";
        conf->index.push_back("index.html");
        conf->ports = 8080;
        conf->hostname = "10.11.4.4";

        addConfig(conf);
    }       
}

void ServerConfig::addConfig(t_config *config)
{
    configs.push_back(config);
}

t_config *ServerConfig::operator[](int num) const
{
    if (num < (int) configs.size())
        return this->configs[num];
    else
        throw(std::exception());
}

size_t ServerConfig::configSize() const
{
    return configs.size();
}