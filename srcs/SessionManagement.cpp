/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SessionManagement.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 15:27:57 by masoares          #+#    #+#             */
/*   Updated: 2024/11/10 18:31:06 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SessionManagement.hpp"

SessionManagement::SessionManagement()
{}

SessionManagement::~SessionManagement()
{}

void SessionManagement::addUser(std::string user, std::string password)
{
    std::string hashed = crypt(password.c_str(), "$6$");
    if (userData[user].empty())
    {
        userData[user] = hashed;
    }
    else
    {
        throw(SessionManagement::UserAlreadyInUseException())
    }
}

std::string SessionManagement::getUserByName(std::string user)
{
    
}

void SessionManagement::deleteUser(std::string user, std::string password)
{
    
}

void SessionManagement::handleLogin(const std::string &user, const std::string &password, HttpResponse &response)
{
    
}

std::string SessionManagement::generateCookie()
{
    std::stringstream ss;
    srand(time(0));
    ss << rand();
    return ss.str();
}

void SessionManagement::handleCookie(HttpRequest &request, HttpResponse &response)
{
    
}

const char * SessionManagement::UserALreadyInUseException::what() const throw()
{
    return "User is already in use";
}