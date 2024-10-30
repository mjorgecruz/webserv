/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:37 by masoares          #+#    #+#             */
/*   Updated: 2024/10/30 16:25:04 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
{}

HttpRequest::~HttpRequest()
{}

void HttpRequest::setRequest(std::string req)
{
    _request = req;
}

void HttpRequest::fillReqProperties()
{
    std::string partial_line;
    std::string prop;
    std::stringstream X(_request);
    
    getline(X, partial_line);
    _requestType = partial_line;

    while (getline(X, partial_line, ':'))
    {
        if (partial_line.empty())
            continue;
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
    std::cout << getRequestType() << std::endl;
    for (std::map<std::string, std::string>::iterator it = _reqProperties.begin(); it != _reqProperties.end(); it++)
    {
        std::cout << it->first << " : " << it->second << std::endl;
    }
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

//EXCEPTIONS

const char *HttpRequest::HttpPageNotFoundException::what() const throw()
{
    return "Error: Page not found";
}

void HttpRequest::setClientFd(int fd)
{
    _clientFd = fd;
}