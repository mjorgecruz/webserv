/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configs.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 15:11:21 by masoares          #+#    #+#             */
/*   Updated: 2024/10/28 16:15:03 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Configs.hpp"

Configs::Configs()
{}
Configs::~Configs()
{
    
}

void Configs::setConfigs(std::string path)
{
    if (path.empty())
    {
        t_config conf;
        conf.host = "10.11.4.4";
        conf.errorPage = "404.html";
        conf.index.push_back("index.html");
        conf.ports = 8080;
        conf.hostname = "10.11.4.4";

        addConfig(conf);
    }       
}

void Configs::addConfig(t_config &config)
{
    configs.push_back(config);
}

t_config Configs::operator[](int num) const
{
    if (num < configs.size())
        return this->configs[num];
    else
        throw(std::exception());
}

size_t Configs::configSize() const
{
    return configs.size();
}