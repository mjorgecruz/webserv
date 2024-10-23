/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 09:45:31 by masoares          #+#    #+#             */
/*   Updated: 2024/10/23 12:08:43 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "webserv.hpp"

int main (void)
{
    int server_socket;

    server_socket = create_server_socket();
    if (server_socket == -1)
        return (1);
	
    fcntl(server_socket, F_SETFL, O_NONBLOCK);
    if (listen(server_socket, SOMAXCONN) == -1)
    {
        std::cerr << "[E] listen failed\n";
        return 1;
    }
    serverings(server_socket);
    close(server_socket);
}
