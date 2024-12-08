/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SessionManagement.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 15:27:57 by masoares          #+#    #+#             */
/*   Updated: 2024/12/08 02:19:53 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SessionManagement.hpp"

SessionManagement::SessionManagement()
{}

SessionManagement::~SessionManagement()
{}

std::string SessionManagement::sessionConfig(HttpRequest &request)
{
    std::string sessionId;
    std::string cookie = request.searchProperty("Cookie");  
    if (cookie != "undefined")
    {
        sessionId = cookie;
    }
    else
    {
        cookie = request.searchProperty("cookie");
        if (cookie != "undefined")
        {
            sessionId = cookie;
        }
        sessionId = generateCookie();
    }
    return sessionId;
}

void SessionManagement::addUser(std::string user, std::string password)
{
    std::string hashed = crypt(password.c_str(), "$6$");
    std::map<std::string,std::string>::iterator it = _userData.find(user);
    if (it == _userData.end())
        _userData[user] = hashed;
    else
        throw(SessionManagement::UserAlreadyInUseException());
}

std::map<std::string,std::string>::iterator SessionManagement::getUserByName(std::string user)
{
    std::map<std::string,std::string>::iterator it = _userData.find(user);
    return it;
}

void SessionManagement::deleteUser(std::string user, std::string password)
{
    std::map<std::string,std::string>::iterator it = _userData.find(user);
    if (it != _userData.end())
    {
        std::string hashed = crypt(password.c_str(), "$6$");
        if (hashed == it->second)
            _userData.erase(it);
        else
            throw(SessionManagement::WrongPasswordException());
    }
}

void SessionManagement::handleLogin(const std::string &user, const std::string &password, HttpResponse &response)
{
    (void) response;
    std::map<std::string,std::string>::iterator it = _userData.find(user);
    if (it != _userData.end())
    {
        std::string hashed = crypt(password.c_str(), "$6$");
        if (hashed == it->second)
            generateCookie();
        else
            throw(SessionManagement::WrongNamePassException());
    }
    else
        throw(SessionManagement::SessionErrorException());
        
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
    (void) request;
    (void) response;
}

const char * SessionManagement::UserAlreadyInUseException::what() const throw()
{
    return "User is already in use";
}

const char * SessionManagement::WrongPasswordException::what() const throw()
{
    return "Wrong Password";
}

const char * SessionManagement::WrongNamePassException::what() const throw()
{
    return "Wrong Name or Password";
}

const char * SessionManagement::SessionErrorException::what() const throw()
{
    return "Server Error - Unauthorized operation";
}