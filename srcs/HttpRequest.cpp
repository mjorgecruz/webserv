/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:37 by masoares          #+#    #+#             */
/*   Updated: 2024/10/24 15:48:13 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "HttpRequest.hpp"

void HttpRequest::setRequest(std::string req)
{
    request = req;
}

void HttpRequest::fillReqProperties()
{
    std::string partial_line;
    std::stringstream X(request);
    
    getline(X, partial_line);
    requestType = partial_line;

    while (getline(X, partial_line, ':'))
    {
        if (partial_line.empty())
            continue;
        
    }
}