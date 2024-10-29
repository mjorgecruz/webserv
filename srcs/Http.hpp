/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 10:22:40 by masoares          #+#    #+#             */
/*   Updated: 2024/10/29 15:24:36 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef HTTP_HPP
# define HTTP_HPP

#include "general.hpp"

class Http
{
    private:
        int _epollFd;
        std::vector<Server *> _listServers;
        HttpRequest request;
        //HttpResponse response;

        Http(const Http &src);
        Http &operator=(const Http & src);
        Http( void );
    
    public:
        ~Http( void );
        Http(const HttpConfig &config);
        void addEpollServer( Server &server );
        void addServersToList(HttpConfig const & config);
        Server *operator[](int num);
        int listServersSize() const;
        void runApplication();
};

#endif