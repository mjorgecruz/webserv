/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:37 by masoares          #+#    #+#             */
/*   Updated: 2024/11/05 19:39:39 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
{}

HttpRequest::~HttpRequest()
{}

void HttpRequest::completeRequest(int socket)
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
            break;
        }
        else
        {
            std::string input(buffer, buffer + bytes_read);
            input = remainder + input;
            remainder = input;
            if (bytes_read < BUFSIZ)
                break;
        }
    }
    remainder = remainder + "\0";
    this->setRequest(remainder);
    this->fillReqProperties();
    this->defineMimeType();

}        

void HttpRequest::setRequest(std::string req)
{
    _request = req;
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

//EXCEPTIONS

const char *HttpRequest::HttpPageNotFoundException::what() const throw()
{
    return "Error: Page not found";
}

std::string HttpRequest::getRequest()
{
    return _request;
}

std::string HttpRequest::getRequestBody()
{
    return _requestBody;
}

std::string HttpRequest::getHeader( void)
{
    return _header;
}