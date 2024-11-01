/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:37 by masoares          #+#    #+#             */
/*   Updated: 2024/10/31 23:57:34 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

HttpResponse::HttpResponse(int client , Server *server)
{
    _client_fd = client;
    _host = server->getHost();
    _port = server->getPorts();


}

HttpResponse::~HttpResponse()
{}


void HttpResponse::setGetHeader()
{
    std::string result;
    result = "";
    if(_status == 200)
        result = "OK";
    std::ostringstream bufferM;
    bufferM << "HTTP/1.1" << _status << result
            << "\r\ncontent-type: " << _contentType
            << "\r\nserver:" << _host << ":" << _port
            << "\r\ncontent-length: " << _contentLength
            << "\r\n\r\n";
    _header = bufferM.str(); 
}

void HttpResponse::setPostHeader()
{
    std::ostringstream bufferM;
    bufferM << "HTTP/1.1" << _status
            << "\r\ncontent-type: " << _contentType
            << "\r\nserver:" << _host << ":" << _port
            << "\r\ncontent-length: " << _contentLength
            << "\r\n\r\n";
    _header = bufferM.str(); 
}

void HttpResponse::setDeleteHeader()
{
    std::ostringstream bufferM;
    bufferM << "HTTP/1.1" << _status
            << "\r\ncontent-type: " << _contentType
            << "\r\nserver:" << _host << ":" << _port
            << "\r\ncontent-length: " << _contentLength
            << "\r\n\r\n";
    _header = bufferM.str(); 
}


void HttpResponse::setContent(std::string content)
{
    _content = content;
}

void HttpResponse::setLength(int len)
{
    _contentLength = len;
}

void HttpResponse::setContentType(std::string type)
{
    _contentType = type;
}

std::string HttpResponse::getHeader()
{
    return _header;    
}

std::string HttpResponse::getContent()
{
    return _content;
}

void HttpResponse::writeContent(std::string path, Server *server)
{
    char buffer[100];
    std::string content = "";
    int fd;
    
    if (path.find_last_of('/') != path.size() - 1)
    {
        try
        {
            path = server->getRoot() + path;
            size_t bytes_read;
            fd = open(path.c_str(), O_RDONLY);
            if (fd == -1)
            {
                if (errno == ENOENT)
                    throw(HttpRequest::HttpPageNotFoundException());
                else if (errno == EACCES)
                    throw(std::exception());
            }
            memset(buffer, 0, 100);
            while ((bytes_read = read(fd, buffer, 99)) > 0)
            {
                content = content + buffer;
                memset(buffer, 0, 100);
            }
            _status = 200;
            setContent(content);
            setLength(content.size());
        }
        catch (HttpRequest::HttpPageNotFoundException &e)
        {
            path = server->getRoot() + server->getErrorPages().find(404)->second;
            _status = 404;
            fd = open(path.c_str(), O_RDONLY);
            if (fd == -1)
            {
                _status = 500;
                throw(std::exception()); //error 500 escrito na string
            }
            memset(buffer, 0, 100);
            while (read(fd, buffer, 99))
            {
                content = content + buffer;
                memset(buffer, 0, 100);
            }
            std::ostringstream bufferM;
            std::string output = bufferM.str();
            setContent(content);
            setLength(content.size());
        }
        catch (std::exception &e)
        {
            path = server->getErrorPages().find(404)->second;
            fd = open(path.c_str(), O_RDONLY);
            if (fd == -1)
            {
                throw(std::exception()); //error 500 escrito na string
            }
            memset(buffer, 0, 100);
            while (read(fd, buffer, 99))
            {
                content = content + buffer;
                memset(buffer, 0, 100);
            }
            setContent(content);
            setLength(content.size());
        }
    }
    else
    {
        try
        {
            if (server->getIndex().empty())
            {
                path = server->getRoot() + "index.html";
                fd = open(path.c_str(), O_RDONLY);
                if (fd == -1)
                {
                    if (errno == ENOENT)
                        throw(HttpRequest::HttpPageNotFoundException());
                    else if (errno == EACCES)
                        throw(std::exception());
                }
            }
            else
            {
                for (size_t i = 0; i < server->getIndex().size(); i++)
                {
                    path = server->getRoot() + "/" + server->getIndex()[i];
                    fd = open(path.c_str(), O_RDONLY);
                    if (fd != -1)
                        break;
                }
                if (fd == -1)
                {
                    throw(std::exception());
                }
            }
            size_t bytes_read;
            memset(buffer, 0, 100);
            while ((bytes_read = read(fd, buffer, 99)) > 0)
            {
                content = content + buffer;
                memset(buffer, 0, 100);
            }
            setContent(content);
            setLength(content.size());
        }
        catch (std::exception &e)
        {
            
        }
    }
    
}
