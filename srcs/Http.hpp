/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-ffe <luis-ffe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 10:22:40 by masoares          #+#    #+#             */
/*   Updated: 2024/10/30 14:43:59 by luis-ffe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HPP
# define HTTP_HPP

#include "general.hpp"

class Server;

class Http
{
    private:
        int _epollFd;
        std::vector<Server *> _listServers;
        HttpRequest request;
        //HttpResponse response;

        Http(const Http &src);
        Http &operator=(const Http & src);
    
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
};

#endif