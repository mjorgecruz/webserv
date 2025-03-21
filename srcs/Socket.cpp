/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-ffe <luis-ffe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 09:52:33 by masoares          #+#    #+#             */
/*   Updated: 2024/12/12 18:16:24 by luis-ffe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

    socket_fd = socket(sa.sin_family, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if( socket_fd == -1)
        throw(std::exception());
    int opt = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); 
    status = bind(socket_fd, (struct sockaddr *)&sa, sizeof(sa));
    if (status == -1)
        throw(std::exception());
        
    if (listen(socket_fd, SOMAXCONN) == -1)
    {
        std::cerr << "[E] listen failed\n";
        throw(std::exception());
    }
    
    _socketFd = socket_fd;
    _addr = sa;
}

int Socket::getSocketFd() const
{
    return _socketFd;
}

const sockaddr_in Socket::getAddr() const
{
    return _addr;
}

//Exceptions

const char *Socket::FdCreationException::what() const throw()
{
    return "Error while creating SocketFd";
}

const char *Socket::BindException::what() const throw()
{
     return "Error while creating Bind";
}

const char *Socket::ListenException::what() const throw()
{
     return "Error while creating SocketFd";
}