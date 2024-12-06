/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 13:50:43 by masoares          #+#    #+#             */
/*   Updated: 2024/12/05 11:42:20 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location( void )
{
    _maxBodySize = 0;
}

Location::~Location( void )
{}

void Location::addIndex(std::string index)
{
    _index.push_back(index);    
}

void Location::setRoot(std::string root)
{
    _root = root;
}

void Location::addAllowedMethods(std::string method)
{
    _allowedMethods.push_back(method);
}

void Location::addErrorPages(int errorNum, std::string error)
{
    _errorPages[errorNum] = error;
}

void Location::setRedirect(std::string redir)
{
    _redirect = redir;
}

void Location::setAutoIndex(int autoIndex)
{
    _autoIndex = autoIndex;
}

std::vector<std::string> Location::getIndex()
{
    return _index;
}

std::string Location::getRoot()
{
    return _root;
}

void Location::setCgiPath(std::string cgiPath)
{
    _cgiPath = cgiPath;
}

std::vector<std::string> Location::getAllowedMethods()
{
    return _allowedMethods;
}

std::map<int, std::string> Location::getErrorPages()
{
    return _errorPages;
}

std::string Location::getCgiPath()
{
    return _cgiPath;
}
std::string Location::getRedirect()
{
    return _redirect;
}

int Location::getAutoIndex()
{
    return _autoIndex;
}

std::string Location::getPath()
{
    return _path;
}

long Location::getMaxBodySize()
{
    return _maxBodySize;
}

void Location::parseLocation(std::string &line, std::ifstream &file)
{
    std::istringstream iss(line);
    std::string locationKeyword, locationPath;

    iss >> locationKeyword;
    if (locationKeyword != "location")
        custtomThrow("ERROR: Location block");
    if (!(iss >> locationPath))
        custtomThrow("ERROR: Location block: path");

    int starCount = 0;    
    for(size_t i = 0; i < locationPath.size(); i++)
    {
        if (locationPath[i] == '*')
            starCount++;
        
        if (starCount > 1)
            custtomThrow("Too many ***** in location");
    }
    if(locationPath[0] != '/' )
        custtomThrow("ERROR: Invalid Location Path");
    _path = locationPath;

    std::string remaining;
    std::getline(iss, remaining);
    remaining.erase(0, remaining.find_first_not_of(" \t"));
    remaining.erase(remaining.find_last_not_of(" \t") + 1);
    
    if (remaining != "{")
    {
        while (std::getline(file, line))
        {
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty() || line.find_first_not_of(" \t") == std::string::npos)
            {
                continue;
            }
            if (line == "{")
            {
                break;
            }
            else
                custtomThrow("ERROR: Location block: expecting '{'");
        }
    }
    bool hasValidKeywords = false;
    while (std::getline(file, line))
    {
        if (line.find_first_not_of(" \t") == std::string::npos)
            continue;
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        if (line == "}")
            break;

        if (line[line.size() - 1] != ';')
            custtomThrow("ERROR: Location block: expecting ';'");
        line = line.substr(0, line.size() - 1);
        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword;

        if (keyword == "index")
        {
            hasValidKeywords = true;
            //last directive - ok
            keywordIndex(iss);
        }
        else if (keyword == "root")
        {
            //last directive - ok
            keywordRoot(iss);
            hasValidKeywords = true;
        }
        else if (keyword == "cgi_path")
        {
            //last directive overrides - ok
            keywordCgiPath(iss);
            hasValidKeywords = true;
        }
        else if (keyword == "allow_methods")
        {
            //last directive overrides  other - ok
            hasValidKeywords = true;
            keywordMethods(iss);
        }
        else if (keyword == "error_page")
        {
            //accepts multiple in multiple lines - ok
            hasValidKeywords = true;
            keywordErrorPages(iss);
        }
        else if (keyword == "return")
        {
            //last directive overrides others - ok
            hasValidKeywords = true;
            keywordReturn(iss);
        }
        else if (keyword == "autoindex")
        {
            //last directive override - ok
            
            hasValidKeywords = true;
            std::string autoIndexValue;
            iss >> autoIndexValue;
            if (autoIndexValue == "on")
                _autoIndex = 1;
            else if (autoIndexValue == "off")
                _autoIndex = 0;
            else
                custtomThrow("ERROR: Location block: autoindex");
            std::string remaining;
            iss >> remaining;
            if (!remaining.empty())
                custtomThrow("ERROR: Location block: autoindex");
        }
        else if (keyword == "max_body_size")
        {
            keywordMaxBodySize(iss);
        }
        else
            custtomThrow("ERROR: Location block: Invalid Keyword");
    }
    if (!hasValidKeywords)
         custtomThrow("ERROR: 'location' block must contain at least one valid keyword");
    if (line != "}")
        custtomThrow("ERROR: Location Block: No closing '}' found.");
}

void Location::keywordIndex(std::istringstream &iss)
{
    std::string indexFile;
    _index.clear();
    while (iss >> indexFile)
    {
        if (indexFile.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.") != std::string::npos)
            custtomThrow("ERROR: Location Block: index");
        addIndex(indexFile);
    }
}

void Location::keywordRoot(std::istringstream &iss)
{
    std::string newRoot;
    iss >> newRoot;
    if (newRoot[0] != '/')
        custtomThrow("ERROR: Location Block: root");

    std::string remaining;
    iss >> remaining;
    if (!remaining.empty())
        custtomThrow("ERROR: Location Block: root");
    setRoot(newRoot);
}

void Location::keywordCgiPath(std::istringstream &iss)
{
    std::string path;
    iss >> path;
    if (path.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/") != std::string::npos)
        custtomThrow("ERROR: Location Block: CGI PATH");

    std::string remaining;
    iss >> remaining;
    if (!remaining.empty())
        custtomThrow("ERROR: Location Block: CGI PATH");
    setCgiPath(path);
}

void Location::keywordMethods(std::istringstream &iss)
{
    int i = 0;
    std::string method;
    _allowedMethods.clear();
    while (iss >> method)
    {
        if (method != "POST" && method != "GET" && method != "DELETE")
            custtomThrow("ERROR: Location Block: allow_methods");
        addAllowedMethods(method);
        i++;
    }
    if  (i > 3 || i == 0)
        custtomThrow("ERROR: Location Block: allow_methods");
}

void Location::keywordErrorPages(std::istringstream &iss)
{
    std::string remaining;
    std::string token;
    std::vector<int> errorCodes;
    std::string errorPage;
    
    while (iss >> token)
    {
        if (isNumeric(token))
        {
            int errorCode = std::atoi(token.c_str());
            if(isValidError(errorCode))
            {
                errorCodes.push_back(errorCode);    
            }
            else
                custtomThrow("ERROR: Location Block: error_page");
        }
        else
        {
            errorPage = token;
            break;
        }
    }
    if (errorCodes.empty())
        custtomThrow("ERROR: Location Block: error_page");
    if (errorPage.empty() || errorPage.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/.") != std::string::npos)
        custtomThrow("ERROR: Location Block: error_page");
    iss >> remaining;
    if (!remaining.empty())
        custtomThrow("ERROR: Location Block: error_page");
    for (std::vector<int>::iterator it = errorCodes.begin(); it != errorCodes.end(); ++it)
    {
        addErrorPages(*it, errorPage);
    }
}

void Location::keywordReturn(std::istringstream &iss)
{
    std::string firstToken, secondToken;
    std::string redir;
    iss >> firstToken;
    if (firstToken.empty())
        custtomThrow("ERROR: Location Block: return");
    if (isNumeric(firstToken))
    {
        int errorCode = std::atoi(firstToken.c_str());
        if(!isValidError(errorCode))
            custtomThrow("ERROR: Location Block: return");
    }
    else
        custtomThrow("ERROR: Location Block: return");

    iss >> secondToken;
    if (!secondToken.empty())
        redir = firstToken + " " + secondToken;
    else
        redir = firstToken;
        
    std::string remaining;
    iss >> remaining;
    if (!remaining.empty())
        custtomThrow("ERROR: Location Block: return");
        
    setRedirect(redir);
}

void Location::keywordMaxBodySize(std::istringstream &iss)
{
    std::string sizeValue, temp, extra;
    iss >> sizeValue;
    char* end;
    errno = 0;
    long maxBodySize = std::strtol(sizeValue.c_str(), &end, 10);
    if (errno == ERANGE || maxBodySize < 0)
    {
        std::cout << "Invalid max_body_size (overflow or negative value): " << sizeValue << "\n";
        throw std::exception();
    }
    if (*end == 'M' && *(end + 1) == '\0')
    {
        maxBodySize *= 1024 * 1024;
    }
    else if (*end != '\0')
    {
        std::cout << "Invalid max_body_size: " << sizeValue << "\n";
        throw std::exception();
    }
    if (maxBodySize <= 0)
    {
        std::cout << "Invalid max_body_size (must be positive): " << sizeValue << "\n";
        throw std::exception();
    }
    iss >> extra;
    if(!extra.empty())
        custtomThrow("ERROR: Server Block: max_body_size");
    _maxBodySize = maxBodySize;
}
