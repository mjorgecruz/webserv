/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:37 by masoares          #+#    #+#             */
/*   Updated: 2024/11/09 01:44:25 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

HttpResponse::HttpResponse(int client , Server *server)
{
    _client_fd = client;
    _host = server->getHost();
    _port = server->getPorts();
    _content = "";

}

HttpResponse::~HttpResponse()
{}

void HttpResponse::setStatus(int status)
{
    _status = status;
}

void HttpResponse::setGetHeader()
{
    std::string result;
    result = "";
    if(_status == 200)
        result = "OK";
    std::ostringstream bufferM;
    bufferM << "HTTP/1.1 " << _status << " " << result
            << "\r\ncontent-type: " << _contentType
            << "\r\nserver:" << _host
            << "\r\ncontent-length: " << _contentLength
            << "\r\n\r\n";
    _header = bufferM.str(); 
}

void HttpResponse::setPostHeader()
{
    std::ostringstream bufferM;
    bufferM << "HTTP/1.1 " << _status
            << "\r\ncontent-type: " << _contentType
            << "\r\nserver: " << _host
            << "\r\ncontent-length: " << _contentLength
            << "\r\n\r\n" << std::endl;
    _header = bufferM.str(); 
}

void HttpResponse::setDeleteHeader()
{
    std::ostringstream bufferM;
    bufferM << "HTTP/1.1" << _status
            << "\r\ncontent-type: " << _contentType
            << "\r\nserver:" << _host
            << "\r\ncontent-length: " << _contentLength
            << "\r\n\r\n";
    _header = bufferM.str(); 
}


void HttpResponse::setContent(std::string content)
{
    _content = content;
}

void HttpResponse::setLength(int len)
{
    _contentLength = len;
}

void HttpResponse::setContentType(std::string type)
{
    _contentType = type;
}

std::string HttpResponse::getHeader()
{
    return _header;    
}

std::string HttpResponse::getContent()
{
    return _content;
}

void HttpResponse::writeContent(std::string path, t_info  &info)
{
    std::string content = "";
    std::fstream file;
    
    if (path.find_last_of('/') != path.size() - 1)
    {
        try
        {
            path = info._root + path;
            
            file.open(path.c_str());
            if (!file.is_open())
                throw(HttpRequest::HttpPageNotFoundException());
            std::string line;
            while (getline(file, line))
            {
                content = content + line;
                content = content + "\n";
            }
            _status = 200;
            setContent(content);
            setLength(content.size());
        }
        catch (HttpRequest::HttpPageNotFoundException &e)
        {
            path = info._errorPages.find(404)->second;
            _status = 404;
            std::fstream file;
            file.open(path.c_str());
            if (!file.is_open())
            {
                _status = 500;
                throw(std::exception());
            }
            std::string line;
            while (getline(file, line))
            {
                content = content + line;
                content = content + "\n";
            }
            setContent(content);
            setLength(content.size());
        }
        catch (std::exception &e)
        {
            path = info._errorPages.find(403)->second;
            std::fstream file;
            file.open(path.c_str());
            if (!file.is_open())
            {
                _status = 500;
                throw(std::exception());
            }
            std::string line;
            while (getline(file, line))
            {
                content = content + line;
                content = content + "\n";
            }
            setContent(content);
            setLength(content.size());
        }
    }
    else
    {
        try
        {
            if (info._index.empty())
            {
                std::fstream file;
                path = info._root + "index.html";
                file.open(path.c_str());
                _status = 200;
                if (!file.is_open())
                {
                    if (errno == ENOENT)
                        throw(HttpRequest::HttpPageNotFoundException());
                    else if (errno == EACCES)
                        throw(std::exception());
                }
            }
            else
            {
                for (size_t i = 0; i < info._index.size(); i++)
                {
                    path = info._root + "/" + info._index[i];
                    file.open(path.c_str());
                    _status = 200;
                    if (file.is_open())
                        break;
                }
                if (!file.is_open())
                {
                    if (errno == ENOENT)
                        throw(std::exception());
                    else if (errno == EACCES)
                        throw(std::exception());
                }
            }
            std::string line;
            while (getline(file, line))
            {
                content = content + line;
                content = content + "\n";
            }
            setContent(content);
            setLength(content.size());
        }
        catch (std::exception &e)
        {
            
        }
    }
    
}


void HttpResponse::handleDataDeletion(std::string path, HttpRequest &request, t_info &info)
{
    (void) request;
    int pathExistance = definePathType(path, info);
    if (pathExistance < 0)
    {
        if (pathExistance == -1)
            throw(std::exception());
        else if (pathExistance == -2)
            throw(std::exception());
        }

    if (path.find_last_of('/') != path.size() - 1)
    {
        std::string filename = info._root + path;
        int result = access(filename.c_str(), W_OK);
        if (result == -1)
        {
            if (errno == EACCES || errno == EROFS )
                throw(std::exception());
            else if (errno == ENOENT)
                throw(std::exception());
        }
        std::remove(filename.c_str());
    }
    else //if it is a folder
    {
        std::string folder = info._root + path;
        if (access(folder.c_str(), W_OK) != 0)
        {
            if (errno == EACCES || errno == EROFS )
                throw(std::exception());
            else if (errno == ENOENT)
                throw(std::exception());
        }
        DIR *delDirectory;
        dirent *dirfile;
        delDirectory = opendir(folder.c_str());
        while ((dirfile = readdir(delDirectory)) != NULL)
        {
            if (dirfile->d_name[0] != '.')
                continue;
            std::string file = path + dirfile->d_name;
            
            //check if any dirfile is a directory
            struct stat entryInfo;
            if (stat(file.c_str(), &entryInfo) == 0)
            {
                if S_ISDIR(entryInfo.st_mode)
                    throw(std::exception());
            }
            
            //check if any dirfile is a deletable file
            if (access(folder.c_str(), W_OK) != 0)
            {
                if (errno == EACCES || errno == EROFS )
                    throw(std::exception());
                else if (errno == ENOENT)
                    throw(std::exception());
            }
        }
        while ((dirfile = readdir(delDirectory)) != NULL)
        {
            if (dirfile->d_name[0] != '.')
                continue;
            std::string file = path + dirfile->d_name;
            remove(file.c_str());
        }
        rmdir(folder.c_str());
    }
}

int HttpResponse::definePathType(std::string &path, t_info &info)
{
    //path will end in "/" if path is of a directory
    path = info._root + path;
    struct stat buf;
    if(stat(path.c_str(), &buf) == -1)
    {
        if (errno == ENOENT)
            return -1;
        else if( errno == EACCES)
            return -2;
    }
    if (S_ISDIR(buf.st_mode))
    {
        if (path.find_last_of('/') != path.size() - 1)
            path = path + "/";
    }
    return 1;
}
