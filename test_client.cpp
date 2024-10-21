/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_client.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 13:55:37 by masoares          #+#    #+#             */
/*   Updated: 2024/10/21 15:34:43 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <stdio.h>

int main(void)
{

    struct addrinfo hints; // Hints or "filters" for getaddrinfo()
    struct addrinfo *res;  // Result of getaddrinfo()
    struct addrinfo *r;    // Pointer to iterate on results
    int status; // Return value of getaddrinfo()
    char buffer[INET6_ADDRSTRLEN]; // Buffer to convert IP address

    memset(&hints, 0, sizeof(hints)); // Initialize the structure
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo("localhost", 0, &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return (2);
    }

    r = res;
    while (r != NULL) {
        void *addr; // Pointer to IP address
        if (r->ai_family == AF_INET) { // IPv4
            // we need to cast the address as a sockaddr_in structure to
            // get the IP address, since ai_addr might be either
            // sockaddr_in (IPv4) or sockaddr_in6 (IPv6)
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)r->ai_addr;
            // Convert the integer into a legible IP address string
            inet_ntop(r->ai_family, &(ipv4->sin_addr), buffer, sizeof buffer);
            printf("IPv4: %s\n", buffer);
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)r->ai_addr;
            inet_ntop(r->ai_family, &(ipv6->sin6_addr), buffer, sizeof buffer);
            printf("IPv6: %s\n", buffer);
        }
        r = r->ai_next; // Next address in getaddrinfo()'s results
    }
    freeaddrinfo(res);
    
    int clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == -1 )
    {
        std::cerr << "Error at socket() " << errno << std::endl;
        return (1);
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0 )
    {
        std::cerr << "Error at connect() " << errno << std::endl;
        return (1);
    }
    const char* message = "Hello, server!";
    send(clientSocket, message, strlen(message), 0);
    
}