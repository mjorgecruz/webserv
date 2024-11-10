/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SessionManagement.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 15:24:18 by masoares          #+#    #+#             */
/*   Updated: 2024/11/10 18:08:54 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "general.hpp"

class SessionManagement
{
    private:
        std::map<std::string,std::string> userData;
        std::map<std::string, std::string> sessions;
        
    public:
        SessionManagement();
        ~SessionManagement();

        void addUser(std::string user, std::string password);
        std::map<std::string,std::string>::iterator getUserByName(std::string user);
        void deleteUser(std::string user, std::string password);
        

        void handleLogin(const std::string &user, const std::string &password, HttpResponse &response);
        std::string generateCookie();
        
        void handleCookie(HttpRequest &request, HttpResponse &response);


        class UserALreadyInUseException: public std::exception
        {
            const char *what() const throw();
        };
        
};