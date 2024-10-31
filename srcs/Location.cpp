/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-ffe <luis-ffe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 13:50:43 by masoares          #+#    #+#             */
/*   Updated: 2024/10/30 10:16:04 by luis-ffe         ###   ########.fr       */
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

void Location::setAutoIndex(bool autoIndex)
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


std::string Location::getRedirect()
{
    return _redirect;
}

bool Location::getAutoIndex()
{
    return _autoIndex;
}

//sei que tenho de dividir isto em mais functions, mas praja caguei
void Location::parseLocation(std::string &line, std::ifstream &file)
{
    std::istringstream iss(line);
    std::string locationKeyword, locationPath;
    iss >> locationKeyword;

    if (!(iss >> locationPath))
    {
        std::cout << "Error: 'location' must be followed by a path.\n";
        throw std::exception();
    }

    size_t bracePos = locationPath.find('{');
    if (bracePos != std::string::npos)
    {
        locationPath = locationPath.substr(0, bracePos);
        _path = locationPath; //path da location
    }
    else
    {
        _path = locationPath; //path da location

        if (!(std::getline(file, line) && line.find("{") != std::string::npos))
        {
            std::cout << "Expected '{' after 'location' keyword\n";
            throw std::exception();
        }
    }

    bool hasValidKeywords = false;

    while (std::getline(file, line))
    {
        line.erase(0, line.find_first_not_of(" \t"));
        if (line == "}")
        {
            break;
        }

        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword;

        if (keyword == "index")
        {
            hasValidKeywords = true;
            std::string indexFile;
            while (iss >> indexFile)
            {
                if (indexFile.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.") != std::string::npos)
                {
                    std::cout << "Error: Invalid index format: " << indexFile << "\n";
                    throw std::exception();
                }
                setIndex(indexFile);
            }
        }
        else if (keyword == "root")
        {
            hasValidKeywords = true;
            iss >> _root;
            if (_root[0] != '/') // de certeza ?
            {
                std::cout << "Error: Invalid root path: " << _root << "\n";
                throw std::exception();
            }
        }
        else if (keyword == "cgi_path")
        {
            hasValidKeywords = true;
            iss >> _cgiPath;
            if (_cgiPath.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/") != std::string::npos) //path sp pode ter estes caracteres?
            {
                std::cout << "Error: Invalid CGI path: " << _cgiPath << "\n";
                throw std::exception();
            }
        }
        else if (keyword == "allow_methods")
        {
            hasValidKeywords = true;
            std::string method;
            while (iss >> method)
            {
                if (method != "POST" && method != "GET" && method != "DELETE")
                {
                    std::cout << "Error: Invalid HTTP method in allow_methods: " << method << "\n";
                    throw std::exception();
                }
                //_allowedMethods.push_back(method);
                addAllowedMethods(method);
            }
        }
        else if (keyword == "error_page")
        {
            hasValidKeywords = true;
            int errorCode;
            std::string errorPage;
            while (iss >> errorCode)
            {
                if (errorCode < 400 || errorCode > 599)
                {
                    std::cout << "Error: Invalid error code: " << errorCode << "\n";
                    throw std::exception();
                }
                iss >> errorPage;
                if (errorPage.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/") != std::string::npos)
                {
                    std::cout << "Error: Invalid error page path: " << errorPage << "\n";
                    throw std::exception();
                }
                addErrorPages(errorCode, errorPage);
            }
        }
        else if (keyword == "return")
        {
            hasValidKeywords = true;
            iss >> _redirect;
            if (_redirect[0] != '/')
            {
                std::cout << "Error: Invalid redirect path: " << _redirect << "\n";
                throw std::exception();
            }
        }
        else if (keyword == "autoindex")
        {
            hasValidKeywords = true;
            std::string autoIndexValue;
            iss >> autoIndexValue;

            if (autoIndexValue == "on")
                setAutoIndex(true);
            else if (autoIndexValue == "off")
                setAutoIndex(false);
            else
            {
                std::cout << "Error: Invalid value for autoindex: " << autoIndexValue << "\n";
                throw std::exception();
            }
        }
        else
        {
            std::cout << "Error: Invalid keyword in location block: " << keyword << "\n";
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

void Location::printLocationConfig() const
{
    std::cout << "  Root: " << _root << std::endl;

    std::cout << "  Index Files: ";
    for (std::vector<std::string>::const_iterator it = _index.begin(); it != _index.end(); ++it)
    {
        std::cout << *it;
        if (it + 1 != _index.end()) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;

    std::cout << "  CGI Path: " << _cgiPath << std::endl;

    std::cout << "  Allowed Methods: ";
    for (std::vector<std::string>::const_iterator it = _allowedMethods.begin(); it != _allowedMethods.end(); ++it)
    {
        std::cout << *it;
        if (it + 1 != _allowedMethods.end()) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;

    std::cout << "  Error Pages:" << std::endl;
    for (std::map<int, std::string>::const_iterator it = _errorPages.begin(); it != _errorPages.end(); ++it)
    {
        std::cout << "    Error " << it->first << ": " << it->second << std::endl;
    }

    std::cout << "  Redirect: " << _redirect << std::endl;

    std::cout << "  Auto Index: " << (_autoIndex ? "on" : "off") << std::endl;
}


//sei que tenho de dividir isto em mais functions, mas praja caguei
