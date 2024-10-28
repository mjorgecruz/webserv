/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 09:45:31 by masoares          #+#    #+#             */
/*   Updated: 2024/10/28 15:21:58 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "general.hpp"

int g_signal = 0;

void sigint_handler(int signal)
{
    if (signal == SIGINT)
    {
        printf("\nIntercepted SIGINT!\n");
        g_signal = 1;
    }
}


int main (int ac, char **av)
{
    
    if ( ac != 2)
        std::cerr << "Error while starting server:\nNo config file";
    
     
    signal(SIGINT, sigint_handler);
    
    Configs configs;
    configs.setConfigs("");
    
    Http webservs(configs);
    
    
    // servers.configServers(configs);
    // sockets.configSockets(servers);
    

    
    int server_socket ;
    
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
