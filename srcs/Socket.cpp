/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 09:52:33 by masoares          #+#    #+#             */
/*   Updated: 2024/10/29 16:30:57 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Socket.hpp"

Socket::Socket()
{
    std::cout << "Socket created" <<std::endl;
}

Socket::~Socket()
{
    std::cout << "Socket destroyed" << std::endl;
}

Socket::Socket(const Socket &src)
{
    if (this == &src)
        return;
    _socketFd = src._socketFd;
    _addr = src._addr;
}

        
Socket & Socket::operator=(const Socket &src)
{
    if (this == &src)
        return *this;
    _socketFd = src._socketFd;
    _addr = src._addr;
    return *this;
}


void Socket::createSocket(int port, std::string host){
    struct sockaddr_in sa;
    int socket_fd;
    int status;

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(host.c_str());
    sa.sin_port = htons(port);

    socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);
    if( socket_fd == -1)
        throw(std::exception());
    status = bind(socket_fd, (struct sockaddr *)&sa, sizeof(sa));
    if (status == -1)
        throw(std::exception());

    fcntl(socket_fd, F_SETFL, O_NONBLOCK);
    if (listen(socket_fd, SOMAXCONN) == -1)
    {
        std::cerr << "[E] listen failed\n";
        throw(std::exception());
    }
    
    _socketFd = socket_fd;
    _addr = sa;
}