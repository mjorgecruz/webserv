/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 10:00:34 by masoares          #+#    #+#             */
/*   Updated: 2024/10/30 16:16:50 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "general.hpp"
#include <string>

void reply(int socket, HttpRequest received)
{
    std::string type;
    std::string path;
    std::string httpVersion;
    std::istringstream request(received.getRequestType());
    std::vector<char> content;
    char buffer[100];
    
    int fd;
    
    request >> type >> path >> httpVersion;
        std::cout << received.getRequestType() << std::endl;
    if (path == "/") {
        path = "/index.html";
    }
    path = "files" + path;
    std::cout << path << std::endl;
    try{
        size_t bytes_read;
        fd = open(path.c_str(), O_RDONLY);
        if (fd == -1)
        {
            throw(HttpRequest::HttpPageNotFoundException());   
        }
        memset(buffer, 0, 100);
        while ((bytes_read = read(fd, buffer, 99)) > 0)
        {
            content.insert(content.end(), buffer, buffer + bytes_read);
            memset(buffer, 0, 100);
        }
        std::ostringstream bufferM;
        bufferM << "HTTP/1.1 200 OK\r\nContent-length: " << content.size()
                << "\r\nContent-Type: " << received.getMimeType() << "\r\n\r\n";

        std::string output = bufferM.str(); 
        send(socket, output.c_str(), output.size(), 0);
        send(socket, content.data(), content.size(), 0);
    }
    catch (HttpRequest::HttpPageNotFoundException &e){
        path = "files/404.html";
        fd = open(path.c_str(), O_RDONLY);
        memset(buffer, 0, 100);
        while (read(fd, buffer, 99))
        {
            size_t len = strlen(buffer);
            content.insert(content.end(), buffer, buffer + len);
            memset(buffer, 0, 100);
        }
        std::ostringstream bufferM;
        bufferM << "HTTP/1.1 404 Not Found\r\nContent-length: " << content.size()
                << "\r\nContent-Type: " << " text/html\r\n\r\n" ;
        std::string output = bufferM.str(); 
        send(socket, output.c_str(), output.size(), 0);
        send(socket, content.data(), content.size(), 0);
    }
    
}