/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_server2.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 09:18:39 by masoares          #+#    #+#             */
/*   Updated: 2024/10/22 14:48:03 by masoares         ###   ########.fr       */
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
#include <unistd.h>

#define PORT 8080

int create_server_socket( void );
void accept_new_connection(int listener_socket, fd_set *all_sockets, int *fd_max);
void read_data_from_socket(int socket, fd_set *all_sockets, int fd_max, int server_socket);

int main (void)
{
    int server_socket;
    int status;

    fd_set all_sockets;
    fd_set read_fds;
    int fd_max;
    struct timeval timer;

    server_socket = create_server_socket();
    if (server_socket == -1)
        return (1);

    status = listen(server_socket, 10);
    if (status != 0)
        return(1);
    
    FD_ZERO(&all_sockets);
    FD_ZERO(&read_fds);
    FD_SET(server_socket, &all_sockets);

    fd_max = server_socket;

    while(1)
    {
        read_fds = all_sockets;
        timer.tv_sec = 2;
        timer.tv_usec = 0;
        
        status = select(fd_max + 1, &read_fds, NULL, NULL, &timer);
        if (status == -1)
            return (1);
        else if (status == 0)
        {
            printf("[Server] Waiting...\n");
            continue;
        }
        
        for (int i = 0; i <= fd_max; i++) 
        {
            if (FD_ISSET(i, &read_fds) != 1)
            {
                continue ;
            }
            if (i == server_socket) {
                accept_new_connection(server_socket, &all_sockets, &fd_max);
            }
            else {
                read_data_from_socket(i, &all_sockets, fd_max, server_socket);
            }
        }
    }
}

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

void accept_new_connection(int server_socket, fd_set *all_sockets, int *fd_max)
{
    int client_fd;
    char msg[BUFSIZ];
    int status;

    client_fd = accept(server_socket, NULL, NULL);
    if (client_fd == -1)
        return;
    FD_SET(client_fd, all_sockets);
    if (client_fd > *fd_max)
        *fd_max = client_fd;
}

void read_data_from_socket(int socket, fd_set *all_sockets, int fd_max, int server_socket)
{
    char buffer[BUFSIZ];
    char msg[BUFSIZ];
    int bytes_read;
    int status;

    memset(&buffer, 0, sizeof buffer);
    bytes_read = recv(socket, buffer, BUFSIZ, 0);
    if (bytes_read <= 0)
    {
        close(socket);
        FD_CLR(socket, all_sockets);
    }
    else
    {
        printf("[%d] Got message: %s", socket, buffer);
        memset(&msg, '\0', sizeof msg);
    }
    
}