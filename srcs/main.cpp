/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-ffe <luis-ffe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 09:45:31 by masoares          #+#    #+#             */
/*   Updated: 2024/11/01 10:45:19 by luis-ffe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
    (void) ac;
    // if (ac != 2)
    //     std::cerr << "Error while starting server:\nNo config file";
    // signal(SIGINT, sigint_handler);

    
    Http http;
    std::cout << "STEP\n";
    http.webservInitializer("webserv.conf");
    return 0;
    //http.runApplication();
    // Http webservs(httpConfigs);

    // webservs.runApplication();
}
