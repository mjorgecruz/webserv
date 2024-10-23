/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:44 by masoares          #+#    #+#             */
/*   Updated: 2024/10/23 16:07:32 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include "webserv.hpp"
#include <map>

class HttpRequest
{
    private:
        std::string request;
        std::string request_type;
        std::map<std::string, std::string> req_properties;
        
    public:
        void setRequest(std::string req);
        void fillReqProperties();
        
        
}


#endif