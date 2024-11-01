/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 10:22:40 by masoares          #+#    #+#             */
/*   Updated: 2024/11/01 09:33:46 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef HTTP_HPP
# define HTTP_HPP

#include "general.hpp"

class Server;
class HttpRequest;
class HttpResponse;
class Location;

class Http
{
    private:
        int _epollFd;
        std::vector<Server *> _listServers;

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
        
        void data_transfer(int socket);
        Server *findCorrespondingServer(int socket);
        void reply(int socket, HttpRequest *received, HttpResponse *response, Server* server);

        std::map<std::string, Location *>::iterator findLocation(std::map<std::string, Location *> &possibleLocations, std::string path);
         

};

#endif