/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConfig.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 10:22:40 by masoares          #+#    #+#             */
/*   Updated: 2024/10/29 15:29:59 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef HTTPCONFIG_HPP
# define HTTPCONFIG_HPP

#include "general.hpp"

class HttpConfig
{
    private:
        
        std::vector<ServerConfig *> _ServerConfigs;
    
    public:
        size_t configSize() const;
        ServerConfig *operator[](int num) const;
        void addToServerConfigs( ServerConfig * server);
};

#endif