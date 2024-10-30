/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:44 by masoares          #+#    #+#             */
/*   Updated: 2024/10/30 16:31:47 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "general.hpp"

class HttpResponse
{
    private:
        int _status;
        //content-length: 20994
        //content-type: text/javascript
        
        std::string _header;
        std::vector<char> _content;
        
    public:
        HttpResponse();
        ~HttpResponse();
        void setHeader();
        void setContent();
        std::string getHeader();
        std::string getContent();
        
        void sendRequest();
};


#endif