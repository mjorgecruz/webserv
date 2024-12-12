/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 09:52:30 by masoares          #+#    #+#             */
/*   Updated: 2024/10/31 22:57:17 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

class Socket
{
    private:
        int _socketFd;
        sockaddr_in _addr;

        Socket(const Socket &src);
        Socket & operator=(const Socket &src);
    public:
        Socket();
        ~Socket();
        void createSocket(int port, std::string host);
        int getSocketFd() const;
        const sockaddr_in getAddr() const;

        class FdCreationException: public std::exception
        {
            virtual const char *what() const throw();
        };

        class BindException: public std::exception
        {
            virtual const char *what() const throw();
        };

        class ListenException: public std::exception
        {
            virtual const char *what() const throw();
        };
        
};
