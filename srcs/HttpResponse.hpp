/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:44 by masoares          #+#    #+#             */
/*   Updated: 2024/11/01 11:57:45 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "general.hpp"

class Server;
class HttpRequest;
class Location;
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
        
        //no location found
        void writeContent(std::string path, Server *server);
        void handleDataUpload(std::string path, HttpRequest &request, Server *server);
        void handleDataDeletion(std::string path, HttpRequest &request, Server *server);

        //location found
        void writeContent(std::string path, Location *location);
        void handleDataUpload(std::string path, HttpRequest &request, Location *location);
        void handleDataDeletion(std::string path, HttpRequest &request, Location *location);
};


#endif