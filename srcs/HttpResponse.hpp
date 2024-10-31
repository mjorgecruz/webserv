/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:44 by masoares          #+#    #+#             */
/*   Updated: 2024/10/31 17:27:35 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "general.hpp"

class Server;
class HttpResponse
{
    private:
        int _client_fd;
        std::string _connection;
        
        int _status;
        int _contentLength;//: 20994
        std::string _contentType;// text/javascript
        std::string _host;
        std::string _port;
        std::string _header;
        std::string _content;
        
    public:
        HttpResponse(int client, Server *server);
        ~HttpResponse();
        void setContent(std::string content);
        void setLength(int len);
        void setContentType(std::string type);
        void setGetHeader();
        void setPostHeader();
        void setDeleteHeader();
        
        std::string getHeader();
        std::string getContent();
        
        
        void writeContent(std::string path, Server *server);
        
        void sendRequest();
};


#endif