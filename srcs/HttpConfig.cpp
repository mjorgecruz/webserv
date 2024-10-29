/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 15:03:39 by masoares          #+#    #+#             */
/*   Updated: 2024/10/29 15:31:40 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "HttpConfig.hpp"

size_t HttpConfig::configSize() const
{
    return _ServerConfigs.size();
}

ServerConfig *HttpConfig::operator[](int num) const
{
    if (num < _ServerConfigs.size())
        return _ServerConfigs[num];
    throw(std::exception());
}

void HttpConfig::addToServerConfigs( ServerConfig * server)
{
    _ServerConfigs.push_back(server);
}