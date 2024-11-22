/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:37 by masoares          #+#    #+#             */
/*   Updated: 2024/11/22 12:12:52 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
{
    _request = "";
    completed = false;
}

HttpRequest::~HttpRequest()
{}

bool HttpRequest::completeRequest(int socket)
{
    std::string remainder = "";
    char buffer[BUFSIZ];
    int bytes_read;
    
    while (1)
    {    
        memset(&buffer, 0, sizeof buffer);
        bytes_read = recv(socket, buffer, BUFSIZ, 0);
        if (bytes_read < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                std::cout << "EAGAIN or EWOULDBLOCK encountered, breaking loop" << std::endl;
                break;
            } 
            else
            {
                std::cerr << "Error reading from socket: " << strerror(errno) << std::endl;
                close(socket);
                return false;
            }
        }
        else if (bytes_read == 0)
        {
            std::cout << "Connection closed by client" << std::endl;
            close(socket);
            return false;
        }
        else
        {
            std::string input(buffer, buffer + bytes_read);
            input = remainder + input;
            remainder = input;
            std::cout << "Read " << bytes_read << " bytes: " << input << std::endl;
        }
    }
    _request += remainder;
    std::cout << "REMAINDER____ \n" << getRequest() << std::endl;
    std::cout << "\n____REMAINDER " << std::endl;
    if (checkRequestEnd())
    {
        return true;
    }
    return false;
}        

void HttpRequest::setRequest(std::string req)
{
    _request = _request + req;
}

void HttpRequest::fillReqProperties()
{
    std::string partial_line;
    std::string prop;

    setHeader();
    setRequestBody();

    std::stringstream X(_header);
    
    getline(X, partial_line);
    _requestType = partial_line;
    std::cout << _requestType <<std::endl;

    while (getline(X, partial_line, ':'))
    {
        if (partial_line.empty())
            break;
        if (_reqProperties.find(partial_line) == _reqProperties.end())
        {
            prop = partial_line;
            if (!getline(X, partial_line))
                break;
            partial_line.erase(std::remove_if(partial_line.begin(), partial_line.end(), isspace), partial_line.end());
            std::pair<std::string, std::string> properties = std::make_pair(prop, partial_line);
            _reqProperties.insert(properties);
        }
    }   
}

void HttpRequest::setRequestBody()
{
    size_t header_end = _request.find("\r\n\r\n");
    if (header_end == std::string::npos)
    {
        header_end = _request.size() - 1;
        _body = "";
    }
    else
        _body = _request.substr(header_end + 4, _request.size() - 1 - header_end + 4);
}

void HttpRequest::setHeader()
{
    size_t header_end = _request.find("\r\n\r\n");
    _header = _request.substr(0, header_end);
    std::cout << _header << std::endl;
}

void HttpRequest::defineMimeType()
{
    std::string type;
    std::string path;
    std::string httpVersion;
    std::string partial_line;
    std::istringstream request(getRequestType());
    std::istringstream accepted (searchProperty("Accept"));

    request >> type >> path >> httpVersion;

    if (path == "/")
    {
        _mimeType = "text/html";
        return;
    }
    std::istringstream mime(path);
    std::string fileType;
    while (getline(mime, partial_line, '.'))
        fileType = partial_line;

    while(getline(accepted, partial_line, ','))
    {
        if(partial_line.find(fileType) != std::string::npos )
        {
            _mimeType = partial_line;
            break;
        }
        else if(partial_line.find("*") != std::string::npos )
        {
            if (partial_line.find("*/*") == std::string::npos)
            {
                _mimeType = "*/*";
                break;
            }
            else
            {
                int i = 0;
                while (partial_line[i] != '*')
                {
                   _mimeType = _mimeType + partial_line[i];
                   i++;
                }
                _mimeType = _mimeType + partial_line[i];
                break;
            }
        }
    }
}

std::string HttpRequest::getRequestType()
{
    return _requestType;
}

std::string HttpRequest::getMimeType()
{
    return _mimeType;
}

std::string HttpRequest::searchProperty(std::string property)
{
    std::map<std::string, std::string>::iterator it = _reqProperties.find(property);
    if (it != _reqProperties.end())
        return it->second;
    return "undefined";
}

void HttpRequest::setClientFd(int fd)
{
    _clientFd = fd;
}

std::string HttpRequest::getRequest()
{
    return _request;
}

std::string HttpRequest::getRequestBody()
{
    return _body;
}

std::string HttpRequest::getHeader( void)
{
    return _header;
}

//EXCEPTIONS

const char *HttpRequest::HttpPageNotFoundException::what() const throw()
{
    return "Error: Page not found";
}

const char *HttpRequest::HttpPageForbiddenException::what() const throw()
{
    return "Error: Forbidden";
}

bool HttpRequest::checkRequestEnd()
{
    size_t init_pos = _request.find("Transfer-Encoding: ");
    if (init_pos == std::string::npos)
    {
        if(_request.find("\r\n\r\n") != std::string::npos)
            return true;
        else
            return false;
    }
    else
    {
        size_t final_pos = _request.find("chunked", init_pos);
        if (final_pos != std::string::npos)
        {
            if(_request.find("0\r\n\r\n") != std::string::npos)
                return true;
            else
                return false;
        }
        else
            return true;
    }
    return true;
}