/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   general.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 14:42:35 by masoares          #+#    #+#             */
/*   Updated: 2024/10/28 11:52:10 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/


#ifndef GENERAL_HPP
# define GENERAL_HPP

#include "webserv.hpp"
#include "Http.hpp"
#include "Server.hpp"
#include "Configs.hpp"
#include "HttpRequest.hpp"


void serverings(int server_socket);
void accept_new_connection(int server_socket, int epoll_fd );
void read_data_from_socket(int socket);
void reply(int socket, HttpRequest received);


#endif

