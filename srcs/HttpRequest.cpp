/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:37 by masoares          #+#    #+#             */
/*   Updated: 2024/10/26 19:46:00 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
{}

HttpRequest::~HttpRequest()
{}

void HttpRequest::setRequest(std::string req)
{
    request = req;
}

void HttpRequest::fillReqProperties()
{
    std::string partial_line;
    std::string prop;
    std::stringstream X(request);
    
    getline(X, partial_line);
    requestType = partial_line;

    while (getline(X, partial_line, ':'))
    {
        if (partial_line.empty())
            continue;
        if (reqProperties.find(partial_line) == reqProperties.end())
        {
            prop = partial_line;
            if (!getline(X, partial_line))
                break;
            std::pair<std::string, std::string> properties = std::make_pair(prop, partial_line);
            reqProperties.insert(properties);
        }
    }

    for (std::map<std::string, std::string>::iterator it = reqProperties.begin(); it != reqProperties.end(); it++)
    {
        std::cout << it->first << " : " << it->second << std::endl;
    }
}

std::string HttpRequest::getRequest()
{
    return request;
}

std::string HttpRequest::searchProperty(std::string property)
{
    std::map<std::string, std::string>::iterator it = reqProperties.find(property);
    if (it != reqProperties.end())
        return it->second;
    return "undefined";
}