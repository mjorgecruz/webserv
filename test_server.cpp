/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 11:23:57 by masoares          #+#    #+#             */
/*   Updated: 2024/10/21 14:30:25 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <netinet/in.h>

int main(void)
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == -1 )
    {
        std::cerr << "Error at socket() " << errno << std::endl;
        return (1);
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(serverSocket,(struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0)
    {
        std::cerr << "Error at bind() " << errno << std::endl;
        close(serverSocket);
        return (1);
    }

    if (listen(serverSocket, 5) != 0)
    {
        std::cerr << "Error at listen() " << errno << std::endl;
        close(serverSocket);
        return (1);
    }
    
    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == -1)
    {
        std::cerr << "Error at accept() " << errno << std::endl;
        close(serverSocket);
        return (1);
    }

    char buffer[1024] = { 0 };
    recv(clientSocket, buffer, sizeof(buffer), 0);
    std::cout << "Message from client: " << buffer << std::endl;

    

    
    close(serverSocket);
    
}