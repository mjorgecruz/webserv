/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 10:00:34 by masoares          #+#    #+#             */
/*   Updated: 2024/10/26 19:40:24 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "general.hpp"
#include <string>

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
    sockets.addVectorSocket(socket_fd, sa);
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
            else if (events[i].events & (EPOLLIN | EPOLLET ))
            {
                read_data_from_socket(events[i].data.fd);
                close(events[i].data.fd);
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
    socklen_t length = 0;
    struct epoll_event event;

    memset(&client, 0, sizeof(client));
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
    HttpRequest request;
    std::string remainder = "";
    char buffer[BUFSIZ];
    int bytes_read;
    
    while (1)
    {    
        memset(&buffer, 0, sizeof buffer);
        bytes_read = recv(socket, buffer, BUFSIZ, 0);
        if (bytes_read < 0)
        {
            break;
        }
        else
        {
            std::string input(buffer, buffer + bytes_read);
            input = remainder + input;
            remainder = input;
            if (bytes_read < BUFSIZ)
                break;
        }
    }
    remainder = remainder + "\0";
    request.setRequest(remainder);
    request.fillReqProperties();
    
    reply(socket, request);
}

void reply(int socket, HttpRequest received)
{
    (void) received;
    
    std::string bufferM;
    std::string msg = "A puta co pariu";
    std::stringstream stream;
    std::string out;
    std::cout << msg.size() <<std::endl;
    stream << msg.size();
    out = stream.str();
    bufferM = "HTTP/1.1 200 OK\r\nContent-length: " + out + "\r\nContent-Type: text/html\r\n\r\n" + msg;
    send(socket, bufferM.c_str(), bufferM.size(), 0);
}