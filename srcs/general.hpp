/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   general.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 14:42:35 by masoares          #+#    #+#             */
/*   Updated: 2024/10/30 10:54:57 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/


#ifndef GENERAL_HPP
# define GENERAL_HPP

extern int g_signal;
extern char **envs; 

#include "webserv.hpp"
#include "Socket.hpp"
#include "Server.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Http.hpp"
#include "Location.hpp"


void accept_new_connection(int server_socket, int epoll_fd );
void read_data_from_socket(int socket);
void reply(int socket, HttpRequest received);


#endif

