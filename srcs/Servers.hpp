/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Servers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 10:22:40 by masoares          #+#    #+#             */
/*   Updated: 2024/10/25 10:38:42 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef SERVERS_HPP
# define SERVERS_HPP

#include "webserv.hpp"

class Servers
{
    private:
        std::vector<int, std::string> listServers;
    
    public:
        Servers( void );
        ~Servers( void );
        void addPairServer( int port, std::string addr );
        void configServers( void );
};

#endif