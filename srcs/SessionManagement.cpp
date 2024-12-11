/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SessionManagement.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 15:27:57 by masoares          #+#    #+#             */
/*   Updated: 2024/12/10 11:57:09 by masoares         ###   ########.fr       */
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
    
    std::string user = line.substr(line.find("username=") + 9, line.find("&password=") - line.find("username=") - 9);
    std::string password = line.substr(line.find("&password=") + 10, line.size() - line.find("&password=") - 10);
    try{
        handleLogin(user, password, sessionId);
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
    {
        _userData[user] = password;
    }
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

void SessionManagement::handleLogout(std::string sessionId)
{
    if (_sessions.find(sessionId) != _sessions.end())
        _sessions.erase(_sessions.find(sessionId));
}


void SessionManagement::handleDelete(std::string fullPath, std::string sessionId, HttpResponse &response, t_info &info)
{
    (void) fullPath;
    (void) response;
    (void) info;
    
    if (_sessions.find(sessionId) != _sessions.end())
    {
        std::string user = _sessions.find(sessionId)->second;
        _userData.erase(user);
        std::ifstream infile((info._root + "/.protected").c_str());
        std::ofstream tempFile((info._root + "/temp").c_str());
        std::string line;
        while (getline(infile, line))
        {
            if (line.find(user + ":") != 0)
            {
                tempFile << line << std::endl;
            }
        }
        infile.close();
        tempFile.close();

        std::remove((info._root + "/.protected").c_str());
        std::rename((info._root + "/temp").c_str(), (info._root + "/.protected").c_str());
    }
    
}

void SessionManagement::handleCreate(std::string fullPath, std::string sessionId, HttpResponse &response, t_info &info)
{   
    (void) sessionId;
    (void) response;
    (void) info;
    std::ifstream create_form(fullPath.c_str());
    std::string line;
    getline(create_form, line);

    std::string user = line.substr(line.find("username=") + 9, line.find("&password=") - line.find("username=") - 9);
    std::string password = line.substr(line.find("&password=") + 10, line.size() - line.find("&password=") - 10);
    addUser(user, password);
    std::ofstream user_file((info._root + "/.protected").c_str(), std::ios::app);
    if (user_file.is_open())
    {
        user_file << user << ":" << password << std::endl;
        user_file.close();
    }
}

std::string SessionManagement::generateCookie()
{
    std::stringstream ss;
    srand(time(0));
    ss << rand();
    return ss.str();
}

bool SessionManagement::checkSession(std::string sessionId)
{
    if (_sessions.find(sessionId) != _sessions.end())
    {
        return true;
    }
    return false;
}

void SessionManagement::fillUsers(std::string authFile)
{
    std::ifstream file(authFile.c_str());
    std::string line;
    
    while(getline(file, line))
    {
        std::string user = line.substr(0, line.find(":"));
        std::string pass = line.substr(line.find(":") + 1, line.size() - 1 - line.find(":"));
        _userData[user] = pass;
    }
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
