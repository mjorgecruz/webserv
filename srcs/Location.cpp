/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-ffe <luis-ffe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 13:50:43 by masoares          #+#    #+#             */
/*   Updated: 2024/12/11 11:11:05 by luis-ffe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location( void )
{
    _maxBodySize = 0;
    _authFile = "";
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

    bool AuthBasic = false;
    bool AuthFile = false;
    iss >> locationKeyword;
    if (locationKeyword != "location")
        custtomLocationThrow("locatin");
    if (!(iss >> locationPath))
        custtomLocationThrow("path");

    int starCount = 0;    
    for(size_t i = 0; i < locationPath.size(); i++)
    {
        if (locationPath[i] == '*')
            starCount++;
        
        if (starCount > 1)
            custtomLocationThrow("Too many ***** in location");
    }
    if(locationPath[0] != '/' )
        custtomLocationThrow("Invalid Location Path");
    this->_path = locationPath;

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
                custtomLocationThrow("expecting '{'");
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
            custtomLocationThrow("expecting ';'");
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
                _autoIndex = 1;
            else if (autoIndexValue == "off")
                _autoIndex = 0;
            else
                custtomLocationThrow("autoindex");
            std::string remaining;
            iss >> remaining;
            if (!remaining.empty())
                custtomLocationThrow("autoindex");
        }
        else if (keyword == "max_body_size")
        {
            hasValidKeywords = true;
            keywordMaxBodySize(iss);
        }
        else if (keyword == "auth_basic")
        {
            hasValidKeywords = true;
            keywordAuthBasic(iss);
            AuthBasic = true;
        }
        else if (keyword == "auth_basic_user_file")
        {
            hasValidKeywords = true;
            keywordAuthFile(iss);
            AuthFile = true;
        }
        else
            custtomLocationThrow("Invalid Keyword");
    }
    if (!hasValidKeywords)
         custtomLocationThrow("Block must contain at least one valid keyword");
    if (line != "}")
        custtomLocationThrow("No closing '}' found.");
    if ((AuthBasic || AuthFile) && (!AuthBasic || !AuthFile))
        custtomLocationThrow("Authentication Keywords missing");
}

void Location::keywordIndex(std::istringstream &iss)
{
    std::string indexFile;
    _index.clear();
    while (iss >> indexFile)
    {
        if (indexFile.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.") != std::string::npos)
            custtomLocationThrow("index");
        addIndex(indexFile);
    }
}

void Location::keywordRoot(std::istringstream &iss)
{
    std::string newRoot;
    iss >> newRoot;
    if (newRoot[0] != '/')
        custtomLocationThrow("root");

    std::string remaining;
    iss >> remaining;
    if (!remaining.empty())
        custtomLocationThrow("root");
    setRoot(newRoot);
}

void Location::keywordCgiPath(std::istringstream &iss)
{
    std::string path;
    iss >> path;
    if (path.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/") != std::string::npos)
        custtomLocationThrow("CGI PATH");

    std::string remaining;
    iss >> remaining;
    if (!remaining.empty())
        custtomLocationThrow("CGI PATH");
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
            custtomLocationThrow("allow_methods");
        addAllowedMethods(method);
        i++;
    }
    if  (i > 3 || i == 0)
        custtomLocationThrow("allow_methods");
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
                custtomLocationThrow("error_page");
        }
        else
        {
            errorPage = token;
            break;
        }
    }
    if (errorCodes.empty())
        custtomLocationThrow("error_page");
    if (errorPage.empty() || errorPage.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/.") != std::string::npos)
        custtomLocationThrow("error_page");
    iss >> remaining;
    if (!remaining.empty())
        custtomLocationThrow("error_page");
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
        custtomLocationThrow("return");
    if (isNumeric(firstToken))
    {
        int errorCode = std::atoi(firstToken.c_str());
        if(!isValidError(errorCode))
            custtomLocationThrow("return");
    }
    else
        custtomLocationThrow("return");

    iss >> secondToken;
    if (!secondToken.empty())
        redir = firstToken + " " + secondToken;
    else
        redir = firstToken;
        
    std::string remaining;
    iss >> remaining;
    if (!remaining.empty())
        custtomLocationThrow("return");
        
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
        custtomLocationThrow("Invalid max_body_size (overflow or negative value)");
    if (*end == 'M' && *(end + 1) == '\0')
    {
        maxBodySize *= 1024 * 1024;
    }
    else if (*end != '\0')
        custtomLocationThrow("Invalid max_body_size: " + sizeValue );
    if (maxBodySize <= 0)
        custtomLocationThrow("Invalid max_body_size (must be positive): " + sizeValue);
    iss >> extra;
    if(!extra.empty())
        custtomLocationThrow("ERROR: Server Block: max_body_size");
    _maxBodySize = maxBodySize;
}

const char *Location::exceptionAtLocation::what(void) const throw()
{
	return ("Error: At Parsing Location");
};


void Location::keywordAuthFile(std::istringstream &iss)
{
    std::string filePath;
    iss >> filePath;
    if (filePath.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/.") != std::string::npos)
        custtomLocationThrow("Authentication File");

    std::string remaining;
    iss >> remaining;
    if (!remaining.empty())
        custtomLocationThrow("Authentication File");
    setAuthFile(filePath);
}

void Location::setAuthFile(std::string file)
{
    this->_authFile = file;

    //check if auth file has the correct features
    struct stat aFile;
    if (stat(file.c_str(), &aFile) == 0)
    {
        std::ifstream authorized(file.c_str());
        authorized.open(file.c_str());
        std::string line;
        while (getline(authorized, line))
        {
            if (line.empty() || line.find(" ") || line.find(":") >= line.size() - 1)
                custtomLocationThrow("AuthFile badly configured");
        }
        return;
    }
    custtomLocationThrow("AuthFile does not exist or permission to open denied");
}

void Location::keywordAuthBasic(std::istringstream &iss)
{
    std::string str;
    std::string remaining;
    iss >> str;

    if (str[0] != '"')
    {
        iss >> remaining;
        if (!remaining.empty())
            custtomLocationThrow("Authentication Basic");   
    }
    else
    {
        bool foundClosingQuote = false;
        if (str[str.length() -1 ] == '"')
        {
            foundClosingQuote = true;
            iss >> remaining;
            if (!remaining.empty())
                custtomLocationThrow("Authentication Basic");
            return ;
        }
        while (iss >> str)
        {
            if (str.find('"') != std::string::npos)
            {
                foundClosingQuote = true;
                iss >> remaining;
                if (!remaining.empty())
                    custtomLocationThrow("Authentication Basic");
                break;
            }
        }
        if (!foundClosingQuote)
            custtomLocationThrow("Authentication Basic");
    }
}

std::string Location::getAuthFile()
{
    return _authFile;
}