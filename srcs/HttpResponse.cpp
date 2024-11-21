/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:37 by masoares          #+#    #+#             */
/*   Updated: 2024/11/20 22:21:50 by masoares         ###   ########.fr       */
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
    if (_status == 403)
    {
        bufferM << "HTTP/1.1 " << _status << " Forbidden"
                << "\r\nContent-type: " << _contentType
                << "\r\nServer: " << _host
                << "\r\nContent-length: " << _contentLength
                << "\r\n\r" << std::endl;
    }
    else
    {
        bufferM << "HTTP/1.1 " << _status
                << "\r\ncontent-type: " << _contentType
                << "\r\nserver: " << _host
                << "\r\ncontent-length: " << _contentLength
                << "\r\n\r" << std::endl;
        
    }
    _header = bufferM.str(); 
}

void HttpResponse::setDeleteHeader()
{
    std::ostringstream bufferM;
    bufferM << "HTTP/1.1" << _status
            << "\r\ncontent-type: " << _contentType
            << "\r\nserver:" << _host
            << "\r\ncontent-length: " << _contentLength
            << "\r\n";
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

int HttpResponse::getLength()
{
    return _contentLength;
}

void HttpResponse::writeContent(std::string path, t_info  &info)
{   
    if (path.find_last_of('/') != path.size() - 1)
    {
        try
        {
            if (info._cgiPath.empty())
                writeNormalPage(path, info);
            else
                writeCgiPage(path, info);
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

void HttpResponse::writeCgiPage(std::string path, t_info  &info)
{
    std::string content = "";
    std::fstream file;
    path = info._root + path;
    std::cout << path << std::endl;
    file.open(path.c_str());
    if (!file.is_open())
        throw(HttpRequest::HttpPageNotFoundException());
    file.close();
    CgiManagement pageCreate;
    pageCreate.solveCgiPhp(path, info, content);
    _status = 200;
    std::cout << content << std::endl;
    
    //find type of response
    size_t h1 = content.find("Content-type: ");
    if (h1 == std::string::npos)
        h1 = content.find("Content-Type: ");
    size_t h2 = content.find("\r\n", h1 + 14);
    if (h2 == std::string::npos)
        h2 = content.find("\n", h1 + 14);
    std::string type = content.substr(h1 + 14 , h2 - h1 - 14);
    std::cout << type << std::endl;
    
    //write content
    size_t header_end = content.find("\r\n\r\n");
    if (header_end == std::string::npos)
    {
        header_end = content.find("\n\n", h2 + 1);
        content = content.substr(header_end + 2, content.size() - 1 - header_end - 2);
    }
    else
        content = content.substr(header_end + 4, content.size() - 1 - header_end - 4);
    std::cout << content << std::endl;
    setContentType(type);
    setContent(content);
    setLength(content.size());
}

void HttpResponse::writeIndexPage(std::string path, t_info  &info)
{
    std::string content = "";
    std::fstream file;
    path = info._root + path;
    std::cout << path << std::endl;
    if (info._index.empty())
    {
        file.open(((path + "index.html")).c_str());
        _status = 200;
        if (!file.is_open())
        {
            if (info._autoIndex > 0)
            {
                writeAutoIndex(path, info);
                return;
            }
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
            {
                writeAutoIndex(path, info);
                return;
            }
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

void HttpResponse::writeAutoIndex(std::string path, t_info &info)
{
    DIR * dir;
    dir = opendir(path.c_str());
    std::cout << path << std::endl;
    
    if (dir)
    {
        dirent *directory;
        std::string content = "<html><body><h1>Index of " + path + "</h1><ul>";
        while ((directory = readdir(dir)) != NULL) 
        {
            std::string name = directory->d_name;
            if (name != "." && name != "..")
            {
                std::string fullPath = info._path + "/" + name;
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

void HttpResponse::writeRedirectContent(t_info &Info, HttpRequest *request)
{
    (void) request;
    std::istringstream ss(Info._redirect);
    std::string strStatus;
    std::string addr;
    ss >> strStatus >> addr;
    std::string content;
    if (strStatus == "301")
    {    
        content = "<html>\n"
                "  <head>\n"
                "    <title>301 Moved Permanently</title>\n"
                "  </head>\n"
                "  <body>\n"
                "    <h1>Moved Permanently</h1>\n"
                "  </body>\n"
                "</html>";
        setStatus(301);
    }
    else if (strStatus == "302")
    {
        content = "<html>\n"
                "  <head>\n"
                "    <title>302 Found</title>\n"
                "  </head>\n"
                "  <body>\n"
                "    <h1>Found</h1>\n"
                "  </body>\n"
                "</html>";
        setStatus(302);
    }
    setContent(content);
    setLength(content.size());
}

void HttpResponse::setGetRedirectHeader(t_info &Info)
{
    std::istringstream ss(Info._redirect);
    std::string addr;
    ss >> addr >> addr;
    std::ostringstream bufferM;
    bufferM << "HTTP/1.1 " << _status << " Moved Permanently"
            << "\r\ncontent-type: " << "text/html"
            << "\r\nLocation: " << "http://" << addr
            << "\r\ncontent-length: " << 0
            << "\r\n\r\n";
    _header = bufferM.str(); 
}