/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SessionManagement.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-ffe <luis-ffe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 15:27:57 by masoares          #+#    #+#             */
/*   Updated: 2024/12/09 10:47:51 by luis-ffe         ###   ########.fr       */
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

void SessionManagement::sessionControl(std::string fullPath, std::string sessionId, HttpResponse &response, t_info &info)
{
    std::ifstream login_form(fullPath.c_str());
    std::string line;
    getline(login_form, line);
    
    std::string user = line.substr(line.find("user=") + 5, line.find("&&password=") - 1 - line.find("user=") - 5);
    std::string password = line.substr(line.find("&&password=") + 11, line.size() - line.find("&&password=") - 11);
    try{
        handleLogin(user, password, sessionId);
        response.setRedirectSession(info, sessionId);
        response.setGetRedirectHeader(info, sessionId);
    }
    catch(SessionManagement::WrongNamePassException &e)
    {
        response.setStatus(401);
        response.writeErrorPage(info, 401);
        response.setPostHeader(sessionId);
    }
    
}

void SessionManagement::addUser(std::string user, std::string password)
{
    std::map<std::string,std::string>::iterator it = _userData.find(user);
    if (it == _userData.end())
        _userData[user] = password;
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
        if (password == it->second)
            _userData.erase(it);
        else
            throw(SessionManagement::WrongPasswordException());
    }
}

void SessionManagement::handleLogin(const std::string &user, const std::string &password, std::string sessionId)
{
    std::map<std::string,std::string>::iterator it = _userData.find(user);
    if (it != _userData.end())
    {
        if (password == it->second)
            _sessions[sessionId] = it->first;
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
