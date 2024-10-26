/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Sockets.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 09:52:33 by masoares          #+#    #+#             */
/*   Updated: 2024/10/26 12:07:03 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Sockets.hpp"

Sockets::Sockets()
{
    std::cout << "Sockets created" <<std::endl;
}

Sockets::~Sockets()
{
    std::cout << "Sockets destroyed" << std::endl;
}

void Sockets::addVectorSocket(int socket, sockaddr_in &sa)
{
    vectorSockets.insert(std::make_pair(socket, sa));
}

void Sockets::closeAllSockets( void )
{
    for (std::map<int, sockaddr_in>::iterator it = vectorSockets.begin(); it != vectorSockets.end(); it++)
        close(it->first);
}