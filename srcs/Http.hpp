/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 10:22:40 by masoares          #+#    #+#             */
/*   Updated: 2024/10/30 14:11:59 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef HTTP_HPP
# define HTTP_HPP

#include "general.hpp"

class Server;

class Http
{
    private:
        int _epollFd;
        std::vector<Server *> _listServers;
        HttpRequest _request;
        //HttpResponse response;
    
    public:
        Http( void );
        ~Http( void );

        void webservInitializer(std::string confPath);
        void addServerToList(Server *server);
        
        void addServersToEpoll( void );
        
        void addEpollServer( Server *server );
        Server *operator[](int num);
        int listServersSize() const;
        void runApplication();

        void accept_new_connection(int server_socket, int epoll_fd );
        
        void read_data_from_socket(int socket);
        void analyzeRequest(Server *server, int socket);
};

#endif