/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 09:52:33 by masoares          #+#    #+#             */
/*   Updated: 2024/10/28 13:44:45 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Server.hpp"

Server::Server()
{
    std::cout << "Server created" <<std::endl;
}

Server::~Server()
{
    std::cout << "Server destroyed" << std::endl;
}

Server::Server(const Server &src)
{
    if (this == &src)
        return;
    _serverFd = src._serverFd;
    _port = src._port;
    _host = src._host;
    _addr = src._addr;
    _hostnames = src._hostnames;
}

        
Server & Server::operator=(const Server &src)
{
    if (this == &src)
        return *this;
    _serverFd = src._serverFd;
    _port = src._port;
    _host = src._host;
    _addr = src._addr;
    _hostnames = src._hostnames;
    return *this;
}

Server::Server(int port){
    struct sockaddr_in sa;
    int socket_fd;
    int status;

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;//inet_addr("10.11.4.4");
    sa.sin_port = htons(PORT);

    socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);
    if( socket_fd == -1)
        throw(std::exception());
    status = bind(socket_fd, (struct sockaddr *)&sa, sizeof(sa));
    if (status == -1)
        throw(std::exception());
    
    _serverFd = socket_fd;
    _port = sa.sin_port;
    _host = sa.sin_addr.s_addr;
    _addr = sa;
}


Server::Server(int port, std::string host){
    struct sockaddr_in sa;
    int socket_fd;
    int status;

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(host.c_str());
    sa.sin_port = htons(PORT);

    socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);
    if( socket_fd == -1)
        throw(std::exception());
    status = bind(socket_fd, (struct sockaddr *)&sa, sizeof(sa));
    if (status == -1)
        throw(std::exception());
    
    _serverFd = socket_fd;
    _port = sa.sin_port;
    _host = sa.sin_addr.s_addr;
    _addr = sa;
}

void Server::setHostname(std::string hostname)
{
    _hostnames.push_back(hostname);
}

const std::vector<std::string> & Server::getHostnames( void )
{
    return _hostnames;
}

const int & Server::getServerFd( void )
{
    return _serverFd;
}