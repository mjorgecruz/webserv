/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Sockets.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 09:52:33 by masoares          #+#    #+#             */
/*   Updated: 2024/10/25 14:50:07 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

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
    (void) socket;
    (void) sa;
}