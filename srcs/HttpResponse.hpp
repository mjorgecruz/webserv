/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-ffe <luis-ffe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:44 by masoares          #+#    #+#             */
/*   Updated: 2024/12/09 10:39:05 by luis-ffe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "webserv.hpp"
#include "Socket.hpp"
#include "Server.hpp"
#include "Location.hpp"
#include "InputHandler.hpp"
#include "HttpRequest.hpp"
#include "SessionManagement.hpp"
#include "CgiManagement.hpp"

class Server;
class HttpRequest;
class Location;

struct t_info_struct;
typedef struct s_info t_info;

class CgiManagement;
class SessionManagement;

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
        void setStatus(int status);
        void setContent(std::string content);
        void setLength(int len);
        void setContentType(std::string type);
        void setGetHeader(std::string sessionId);
        void setPostHeader(std::string sessionId);
        void setDeleteHeader(std::string sessionId);
        
        std::string getHeader();
        std::string getContent();
        int getLength();
        
        void writeContent(std::string path, t_info &info, HttpRequest &request);
        
        void writeNormalPage(std::string path, t_info  &info);
        void writeCgiPage(std::string path, t_info  &info, HttpRequest &request);
        
        void writeIndexPage(std::string path, t_info  &info);
        void writeErrorPage(t_info &info, int error);

        void writeFailError();
        void writeAutoIndex(std::string path, t_info &info);

        void writeRedirectContent(t_info &Info, HttpRequest *request);
        void setGetRedirectHeader(t_info &Info, std::string sessionId);

        void setRedirectSession(t_info &Info, std::string sessionId);
        
};


#endif