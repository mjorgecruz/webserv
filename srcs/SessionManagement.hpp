/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SessionManagement.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 15:24:18 by masoares          #+#    #+#             */
/*   Updated: 2024/12/09 21:45:43 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "InputHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class HttpResponse;
class HttpRequest;

struct t_info_struct;
typedef struct s_info t_info;

typedef struct s_session
{
    std::string sessionId;
    
}   t_session;

class SessionManagement
{
    private:
        std::map<std::string,std::string> _userData;
        std::map<std::string, std::string> _sessions;
        
    public:
        
        SessionManagement();
        ~SessionManagement();

        std::string sessionConfig(HttpRequest &request);
        void sessionControl(std::string fullPath, std::string sessionId, HttpResponse &response, t_info &info);

        void addUser(std::string user, std::string password);
        std::map<std::string,std::string>::iterator getUserByName(std::string user);
        void deleteUser(std::string user, std::string password);

        void handleLogin(const std::string &user, const std::string &password, std::string sessionId);
        void handleLogout(std::string sessionId);
        void handleDelete(std::string fullPath, std::string sessionId, HttpResponse &response, t_info &info);
        void handleCreate(std::string fullPath, std::string sessionId, HttpResponse &response, t_info &info);

        
        std::string generateCookie();

        bool checkSession(std::string sessionId);
        void fillUsers(std::string authFile);
        

        class UserAlreadyInUseException: public std::exception
        {
            const char *what() const throw();
        };
        
        class WrongPasswordException: public std::exception
        {
            const char *what() const throw();
        };

        class WrongNamePassException: public std::exception
        {
            const char *what() const throw();
        };

        class SessionErrorException: public std::exception
        {
            const char *what() const throw();
        };
};