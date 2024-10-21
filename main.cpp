/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 12:00:53 by masoares          #+#    #+#             */
/*   Updated: 2024/10/21 14:51:01 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

// C++ program to illustrate the client application in the
// socket programming
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <cstring>

int main(void)
{
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = inet_addr("10.11.4.2");

    connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    std::string line;
    while (true)
    {
        char response[1024] = {0};
        recv(clientSocket, response, sizeof(response), 0);
        std::cout << response << std::endl;
        if (std::getline(std::cin, line))
            send(clientSocket, line.c_str(), line.size(), 0);
    }
    close(clientSocket);
}