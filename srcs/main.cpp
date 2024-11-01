/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 09:45:31 by masoares          #+#    #+#             */
/*   Updated: 2024/11/01 14:55:17 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "general.hpp"

int g_signal = 0;
char **envs = environ; 

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
    {
        std::cerr << "Error while starting server: No config file" << std::endl;
        std::cerr << "Default configuration used" << std::endl;
        std::cerr << std::endl;
    }
    signal(SIGINT, sigint_handler);

    
    Http http;
    
    //run server configurations
    try{
        http.webservInitializer(av[1]);
    }
    catch(std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    
    //run servers
    http.runApplication();
}
