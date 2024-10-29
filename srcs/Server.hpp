/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 09:52:30 by masoares          #+#    #+#             */
/*   Updated: 2024/10/29 11:30:57 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef Server_HPP
# define Server_HPP

#include "webserv.hpp"

class Server
{
    private:
        int _serverFd;
        int _port;
        std::string _host;
        sockaddr_in _addr;
        std::vector<std::string> _hostnames;

        Server(const Server &src);
        Server & operator=(const Server &src);
        Server();
    public:
        ~Server();
        Server(int port);
        Server(int port, std::string host);
        void setHostname(std::string hostname);
        const std::vector<std::string> &getHostnames( void );
        
        const int &getServerFd();
        
    
};

#endif