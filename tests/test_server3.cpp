/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_server3.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 13:54:37 by masoares          #+#    #+#             */
/*   Updated: 2024/10/23 10:10:40 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>

#define PORT 8080
#define MAX_EVENTS 5


int create_server_socket( void )
{
    struct sockaddr_in sa;
    int socket_fd;
    int status;

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;//inet_addr("10.11.4.4");
    sa.sin_port = htons(PORT);

    socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);
    if( socket_fd == -1)
        return(-1);
    status = bind(socket_fd, (struct sockaddr *)&sa, sizeof(sa));
    if (status == -1)
        return (-1);
    return (socket_fd);
}

void accept_new_connection(int server_socket, int epoll_fd )
{
    int client_fd;
    sockaddr client;
    socklen_t length;
    char msg[BUFSIZ];
    int status;
    struct epoll_event event;

    std::cout<< "lala" << std::endl;
    client_fd = accept(server_socket, &client, &length);
    if (client_fd == -1)
    {
        std::cout<< "fd negativo" << std::endl;
        return;
    }
    event.data.fd = client_fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
    std::cout<< "Added: " << client_fd << std::endl;
}

void read_data_from_socket(int socket, int server_socket, struct epoll_event events[MAX_EVENTS], int epoll_fd )
{
    char buffer[BUFSIZ];
    std::string msg = "A puta co pariu";
    int bytes_read;
    int status;
    std::string bufferM;
    memset(&buffer, 0, sizeof buffer);
    bytes_read = recv(socket, buffer, BUFSIZ, 0);
    if (bytes_read <= 0)
    {
        close(socket);
    }
    else
    {
        std::cout << socket << " Got message: " << buffer;
    }
    bufferM = "HTTP/1.1 200 OK\nContent-length: " + std::to_string(msg.size()) + "\nContent-Type: text/html" + "\n\n" + msg;
    send(socket, bufferM.c_str(), bufferM.size(), 0);
    
}


int main (void)
{
    int server_socket;
    int status;
    int event_count;
    int i;

    fd_set all_sockets;
    fd_set read_fds;
    int fd_max;
    struct timeval timer;

    server_socket = create_server_socket();
    if (server_socket == -1)
        return (1);
	
    
    if (listen(server_socket, SOMAXCONN) == -1)
    {
        std::cerr << "[E] listen failed\n";
        return 1;
    }
    
    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];
	int epoll_fd = epoll_create(1);

	if (epoll_fd == -1) {
		fprintf(stderr, "Failed to create epoll file descriptor\n");
		return 1;
	}
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP;
	event.data.fd = server_socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &event) == -1)
    {
        std::cerr << "[E] epoll_ctl failed\n";
        return 1;
    }

    while (true)
    {
        event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, 30000);
        
        for (i = 0; i < event_count; i++) 
        {
            if (events[i].events  & (EPOLLRDHUP | EPOLLERR | EPOLLHUP))
            {
                std::cout << "Page was hard refreshed" << std::endl;
                close(events[i].data.fd);
            }
            else if(events[i].data.fd == server_socket)
            {
                accept_new_connection(server_socket, epoll_fd);
            }
            else if (events[i].events & (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP))
            {
                std::cout << "VAI" << std::endl;
                read_data_from_socket(events[i].data.fd, server_socket, events, epoll_fd );
            }
		}
    }

	if (close(epoll_fd)) {
		fprintf(stderr, "Failed to close epoll file descriptor\n");
		return 1;
	}
    close(server_socket);


}
