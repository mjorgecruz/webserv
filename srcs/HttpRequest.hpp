/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:44 by masoares          #+#    #+#             */
/*   Updated: 2024/10/31 11:05:05 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

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
        std::map<std::string, std::string> _reqProperties;
        std::string _mimeType;
        
    public:
        HttpRequest();
        ~HttpRequest();
        void completeRequest(int socket);
        void setRequest(std::string req);
        void setClientFd(int fd);
        void fillReqProperties();
        void defineMimeType();
        std::string getRequestType();
        std::string searchProperty(std::string property);
        std::string getMimeType();

        
        class HttpPageNotFoundException: public std::exception
        {
            virtual const char *what() const throw();
        };
};


#endif