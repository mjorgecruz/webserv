/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 10:00:34 by masoares          #+#    #+#             */
/*   Updated: 2024/10/23 12:08:52 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "webserv.hpp"

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

void serverings(int server_socket)
{
    int event_count;
    int i;
    
    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];
	int epoll_fd = epoll_create(1);

	if (epoll_fd == -1) {
		fprintf(stderr, "Failed to create epoll file descriptor\n");
		return;
	}
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP;
	event.data.fd = server_socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &event) == -1)
    {
        std::cerr << "[E] epoll_ctl failed\n";
        return;
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
                read_data_from_socket(events[i].data.fd);
            }
		}
    }

	if (close(epoll_fd)) {
		fprintf(stderr, "Failed to close epoll file descriptor\n");
		return;
	}
}

void accept_new_connection(int server_socket, int epoll_fd )
{
    int client_fd;
    sockaddr client;
    socklen_t length;
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

void read_data_from_socket(int socket)
{
    char buffer[BUFSIZ];
    std::string msg = "A puta co pariu";
    int bytes_read;
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
    std::stringstream stream;
    std::string out;
    stream << msg.size();
    stream >> out;
    bufferM = "HTTP/1.1 200 OK\nContent-length: " + out + "\nContent-Type: text/html" + "\n\n" + msg;
    send(socket, bufferM.c_str(), bufferM.size(), 0);
    
}