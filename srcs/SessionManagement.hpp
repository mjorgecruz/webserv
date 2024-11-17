/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SessionManagement.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 15:24:18 by masoares          #+#    #+#             */
/*   Updated: 2024/11/17 16:55:42 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#pragma once

#include "general.hpp"

class SessionManagement
{
    private:
        std::map<std::string,std::string> _userData;
        std::map<std::string, std::string> _sessions;
        
    public:
        SessionManagement();
        ~SessionManagement();

        void addUser(std::string user, std::string password);
        std::map<std::string,std::string>::iterator getUserByName(std::string user);
        void deleteUser(std::string user, std::string password);
        

        void handleLogin(const std::string &user, const std::string &password, HttpResponse &response);
        std::string generateCookie();
        
        void handleCookie(HttpRequest &request, HttpResponse &response);

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