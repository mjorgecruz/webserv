/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Sockets.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 09:52:30 by masoares          #+#    #+#             */
/*   Updated: 2024/10/26 11:49:42 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETS_HPP
# define SOCKETS_HPP

#include "webserv.hpp"


class Sockets
{
    private:
        std::map<int, sockaddr_in> vectorSockets;

    public:
        Sockets();
        ~Sockets();
        void addVectorSocket(int socket, sockaddr_in &sa);
        void closeAllSockets( void );
};

#endif