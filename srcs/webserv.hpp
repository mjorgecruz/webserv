/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 09:46:35 by masoares          #+#    #+#             */
/*   Updated: 2024/10/25 10:06:06 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <sstream>
#include <vector>
#include <map>

#define PORT 8080
#define MAX_EVENTS 128

int create_server_socket( void );
void serverings(int server_socket);
void accept_new_connection(int server_socket, int epoll_fd );
void read_data_from_socket(int socket);
void reply(int socket, std::string received);

#endif