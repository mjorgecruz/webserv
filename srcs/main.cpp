/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 09:45:31 by masoares          #+#    #+#             */
/*   Updated: 2024/10/29 13:33:45 by masoares         ###   ########.fr       */
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
    (void) av;
    if (ac != 2)
        std::cerr << "Error while starting server:\nNo config file";
    signal(SIGINT, sigint_handler);
    
    ServerConfig configs;
    configs.setConfigs("");
    
    Http webservs(configs);

    webservs.runApplication();
    for (size_t i = 0; i < configs.configSize(); i++) 
        delete configs[i];
}
