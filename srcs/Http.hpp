/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 10:22:40 by masoares          #+#    #+#             */
/*   Updated: 2024/12/08 02:37:38 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HPP
# define HTTP_HPP

#include "SessionManagement.hpp"
#include "InputHandler.hpp"
#include "general.hpp"

class Server;
class HttpRequest;
class HttpResponse;
class Location;
class InputHandler;
class SessionManagement;

class Http
{
    private:
        int _epollFd;
        std::vector<Server *> _listServers;

    public:
        SessionManagement allSessions;
        
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
        
        void data_transfer(int socket, struct epoll_event &event, HttpRequest *request);
        std::vector<Server *> findCorrespondingServer(int socket);
        Server * findCorrectServerName(HttpRequest *request, std::vector<Server *> &correspondingServers);
        void reply(int socket, HttpRequest *received, HttpResponse *response, Server* server, std::string sessionId);

        void fillStructInfo(t_info &Info, Server *server, Location *location);
        
        std::vector<std::pair <std::string, Location *> >::iterator findLocation(std::vector<std::pair <std::string, Location *> > &possibleLocations, std::string &path);
        
        void sendData(int socket, HttpResponse *response);

        
        class MaxBodySizeException: public std::exception
        {
            public:
                const char *what( void ) const throw();
        };
};

#endif