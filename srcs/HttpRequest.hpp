/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:44 by masoares          #+#    #+#             */
/*   Updated: 2024/11/30 21:21:37 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include "general.hpp"
#include <map>

class HttpRequest
{
    private:
        int _clientFd;
        std::string _request;
        std::string _requestType;
        std::string _header;
        std::string _body;
        std::map<std::string, std::string> _reqProperties;
        std::string _mimeType;
        
    public:
        bool completed;
        HttpRequest();
        ~HttpRequest();
        bool completeRequest(int socket);
        void setRequest(std::string req);
        void setClientFd(int fd);
        void fillReqProperties();
        void defineMimeType();
        void setRequestBody();
        std::string chunkCleaning(std::string completeBody);
        void setHeader();
        std::string getRequestType();
        std::string searchProperty(std::string property);
        std::string getMimeType();
        std::string getRequest();
        std::string getRequestBody();
        std::string getHeader();

        bool checkRequestEnd();

        class HttpPageNotFoundException: public std::exception
        {
            virtual const char *what() const throw();
        };

        class HttpPageForbiddenException: public std::exception
        {
            virtual const char *what() const throw();
        };        
};


#endif