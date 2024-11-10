/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:37 by masoares          #+#    #+#             */
/*   Updated: 2024/11/10 02:13:39 by masoares         ###   ########.fr       */
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
    if (path.find_last_of('/') != path.size() - 1)
    {
        try
        {
            writeNormalPage(path, info);
        }
        catch (HttpRequest::HttpPageNotFoundException &e)
        {
            writePage404(info);
        }
        catch (HttpRequest::HttpPageForbiddenException &e)
        {
            writePage403(info);
        }
    }
    else
    {
        try
        {
            writeIndexPage(path, info);
        }
        catch (HttpRequest::HttpPageNotFoundException &e)
        {
            writePage404(info);
        }
        catch (HttpRequest::HttpPageForbiddenException &e)
        {
            writePage403(info);
        }
    }
    
}

void HttpResponse::writeNormalPage(std::string path, t_info  &info)
{
    std::string content = "";
    std::fstream file;
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

void HttpResponse::writeIndexPage(std::string path, t_info  &info)
{
    std::string content = "";
    std::fstream file;
    path = info._root + path;
    if (info._index.empty())
    {
        std::fstream file;
        file.open(((path + "index.html")).c_str());
        _status = 200;
        if (!file.is_open())
        {
            if (info._autoIndex > 0)
                writeAutoIndex(path);
            else if (errno == ENOENT)
                throw(HttpRequest::HttpPageNotFoundException());
            else if (errno == EACCES)
                throw(HttpRequest::HttpPageForbiddenException());
        }
    }
    else
    {
        for (size_t i = 0; i < info._index.size(); i++)
        {
            file.open((path + info._index[i]).c_str());
            _status = 200;
            if (file.is_open())
                break;
        }
        if (!file.is_open())
        {
            if (info._autoIndex > 0)
                writeAutoIndex(path);
            if (errno == ENOENT)
                throw(HttpRequest::HttpPageNotFoundException());
            else if (errno == EACCES)
                throw(HttpRequest::HttpPageForbiddenException());
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

void HttpResponse::writePage404(t_info &info)
{
    std::string content = "";
    std::string path;
    path = info._errorPages.find(404)->second;
    _status = 404;
    std::fstream file;
    file.open(path.c_str());
    if (!file.is_open())
    {
        _status = 500;
        writeFailError();
        return;
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

void HttpResponse::writePage403(t_info &info)
{
    std::string content = "";
    std::string path;
    path = info._errorPages.find(403)->second;
    std::fstream file;
    file.open(path.c_str());
    if (!file.is_open())
    {
        _status = 500;
        writeFailError();
        return;
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

void HttpResponse::writeFailError()
{
     std::string response = 
        "<html><body><h1>500 Internal Server Error</h1><p>Server misconfiguration.</p></body></html>";
    setContent(response);
    setLength(response.size());
}

void HttpResponse::writeAutoIndex(std::string path)
{
    DIR * dir;
    dir = opendir(path.c_str());
    if (dir)
    {
        dirent *directory;
        std::string content = "<html><body><h1>Index of " + path + "</h1><ul>";
        while ((directory = readdir(dir)) != NULL) 
        {
            std::string name = directory->d_name;
            if (name != "." && name != "..")
            {
                std::string fullPath = path + name;
                content += "<li><a href=\"" + fullPath + "\">" + name + "</a></li>";
            }            
        }
        content += "</ul></body></html>";
        setContent(content);
        setLength(content.size());
        
        closedir(dir);
    }
    else
        writeFailError();
}