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
