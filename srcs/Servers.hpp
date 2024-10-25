/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Servers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 10:22:40 by masoares          #+#    #+#             */
/*   Updated: 2024/10/25 14:47:50 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef SERVERS_HPP
# define SERVERS_HPP

#include "webserv.hpp"

class Servers
{
    private:
        std::map<int, std::string> listServers;
    
    public:
        Servers( void );
        ~Servers( void );
        void addPairServer( int port, std::string addr );
        void configServers( void );
};

#endif