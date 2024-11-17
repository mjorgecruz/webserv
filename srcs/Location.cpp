/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-ffe <luis-ffe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 13:50:43 by masoares          #+#    #+#             */
/*   Updated: 2024/11/17 10:35:40 by luis-ffe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location( void )
{}

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

// bool isNumeric(const std::string &str)
// {
//     for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
//     {
//         if (!std::isdigit(*it)) {
//             return false;
//         }
//     }
//     return true;
// }

void Location::parseLocation(std::string &line, std::ifstream &file)
{
    std::istringstream iss(line);
    std::string locationKeyword, locationPath;

    iss >> locationKeyword;
    if (locationKeyword != "location") {
        std::cout << "Error: Expected 'location' keyword.\n";
        throw std::exception();
    }

    if (!(iss >> locationPath)) {
        std::cout << "Error: 'location' must be followed by a path.\n";
        throw std::exception();
    }
    
    int starCount = 0;
    
    for(size_t i = 0; i < locationPath.size(); i++)
    {
        if (locationPath[i] == '*')
            starCount++;
        
        if (starCount > 1)
        {
            std::cout << "Too many ***** in location\n";
            throw std::exception();
        }
    }
    _path = locationPath;
    std::string remaining;
    std::getline(iss, remaining);
    remaining.erase(0, remaining.find_first_not_of(" \t"));
    remaining.erase(remaining.find_last_not_of(" \t") + 1);
    
    if (remaining == "{")
    {
        //jumps
    }
    else
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
            {
                std::cout << "Error: Expected '{' after 'location' path.\n";
                throw std::exception();
            }
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
        {
            std::cout << "Error: Missing ';' at the end of line for keyword.\n";
            throw std::exception();
        }
        line = line.substr(0, line.size() - 1);
        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword;

        if (keyword == "index")
        {
            hasValidKeywords = true;
            keywordIndex(iss);
        }
        else if (keyword == "root")
        {
            keywordRoot(iss);
            hasValidKeywords = true;
        }
        else if (keyword == "cgi_path")
        {
            keywordCgiPath(iss);
            hasValidKeywords = true;
        }
        else if (keyword == "allow_methods")
        {
            hasValidKeywords = true;
            keywordMethods(iss);
        }
        else if (keyword == "error_page")
        {
            hasValidKeywords = true;
            keywordErrorPages(iss);
        }
        else if (keyword == "return")
        {
            hasValidKeywords = true;
            keywordReturn(iss);
        }
        else if (keyword == "autoindex")
        {
            hasValidKeywords = true;
            std::string autoIndexValue;
            iss >> autoIndexValue;
            if (autoIndexValue == "on")
            {
                _autoIndex = 1;
            }
            else if (autoIndexValue == "off")
            {
                _autoIndex = 0;
            }
            else
            {
                std::cout << "Invalid value for autoindex: " << autoIndexValue << "\n";
                throw std::exception();
            }
            std::string remaining;
            iss >> remaining;
            if (!remaining.empty())
            {
                std::cout << "ERROR: KEYWORD: [autoindex]" << std::endl;
                throw std::exception();
            }
        }
        else
        {
            std::cout << "Invalid keyword in location block: " << keyword << "\n";
            throw std::exception();
        }
    }
    if (!hasValidKeywords)
    {
        std::cout << "Error: 'location' block must contain at least one valid keyword\n";
        throw std::exception();
    }
    if (line != "}")
    {
        std::cout << "Error: Expected '}' at the end of location block\n";
        throw std::exception();
    }
}

void Location::keywordIndex(std::istringstream &iss)
{
    std::string indexFile;
    while (iss >> indexFile)
    {
        if (indexFile.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.") != std::string::npos)
        {
            std::cout << "Invalid index format: " << indexFile << "\n";
            throw std::exception();
        }
        addIndex(indexFile);
    }
}

void Location::keywordRoot(std::istringstream &iss)
{
    std::string newRoot;
    iss >> newRoot;
    if (newRoot[0] != '/')
    {
        std::cout << "Invalid root path: " << newRoot << "\n";
        throw std::exception();
    }
    std::string remaining;
    iss >> remaining;
    if (!remaining.empty())
    {
        std::cout << "ERROR: KEYWORD: [cgi_path]" << std::endl;
        throw(std::exception());
    }
    setRoot(newRoot);
}

void Location::keywordCgiPath(std::istringstream &iss)
{
    std::string path;
    iss >> path;
    if (path.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/") != std::string::npos)
    {
        std::cout << "Invalid CGI path: " << path << "\n";
        throw std::exception();
    }

    std::string remaining;
    iss >> remaining;
    if (!remaining.empty())
    {
        std::cout << "ERROR: KEYWORD: [cgi_path]" << std::endl;
        throw(std::exception());
    }
    setCgiPath(path);
}

void Location::keywordMethods(std::istringstream &iss)
{
    int i = 0;
    std::string method;
    while (iss >> method)
    {
        if (method != "POST" && method != "GET" && method != "DELETE")
        {
            std::cout << "Invalid HTTP method in allow_methods: " << method << "\n";
            throw std::exception();
        }
        addAllowedMethods(method);
        i++;
    }
    if  (i > 3 || i == 0)
    {
        std::cout << "ERROR: Invalid method count";
        throw std::exception();
    }
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
            {
                std::cout << "ERROR: Invalid Code\n";
                throw std::exception();
            }
        }
        else
        {
            errorPage = token;
            break;
        }
    }
    
    if (errorCodes.empty())
    {
        std::cout << "Invalid error: " << errorPage << "\n";
        throw std::exception();
    }
    if (errorPage.empty() || errorPage.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/.") != std::string::npos)
    {
        std::cout << "Invalid error page path: " << errorPage << "\n";
        throw std::exception();
    }
    
    iss >> remaining;
    if (!remaining.empty())
    {
        std::cout << "ERROR: KEYWORD: [erro_pages] cannot accept more than 1 erro_page" << std::endl;
        throw(std::exception());
    }
    
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
    {
        std::cout << "ERROR: Expected value after return keyword\n";
        throw std::exception();
    }
    
    if (isNumeric(firstToken))
    {
        int errorCode = std::atoi(firstToken.c_str());
        if(!isValidError(errorCode))
        {
            std::cout << "ERROR: Invalid CODE in return Keyword\n";
            throw std::exception();
        }
    }
    else
    {
        std::cout << "ERROR: Non Numeric CODE in return Keyword\n";
        throw std::exception();
    }
    
    iss >> secondToken;
    if (!secondToken.empty())
    {
        redir = firstToken + " " + secondToken;
    }
    else
    {
        redir = firstToken;
    }
    
    std::string remaining;
    iss >> remaining;
    if (!remaining.empty())
    {
        std::cout << "ERROR: KEYWORD: [return] cannot accept more than 2 variables" << std::endl;
        throw(std::exception());
    }
    setRedirect(redir);
}

/*Funtion to print all the configurations of each location class*/

void Location::printLocationConfig() const {
    std::cout << "  Root: " << _root << std::endl;
    std::cout << "  Index Files: ";
    for (std::vector<std::string>::const_iterator it = _index.begin(); it != _index.end(); ++it) {
        std::cout << *it;
        if (it + 1 != _index.end()) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
    std::cout << "  CGI Path: " << _cgiPath << std::endl;
    std::cout << "  Allowed Methods: ";
    for (std::vector<std::string>::const_iterator it = _allowedMethods.begin(); it != _allowedMethods.end(); ++it) {
        std::cout << *it;
        if (it + 1 != _allowedMethods.end()) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
    std::cout << "  Error Pages:" << std::endl;
    for (std::map<int, std::string>::const_iterator it = _errorPages.begin(); it != _errorPages.end(); ++it) {
        std::cout << "    Error " << it->first << ": " << it->second << std::endl;
    }
    std::cout << "  Redirect: " << _redirect << std::endl;
    std::cout << "  Auto Index: " << _autoIndex << std::endl;
}
