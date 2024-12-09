/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:37 by masoares          #+#    #+#             */
/*   Updated: 2024/12/09 22:28:16 by masoares         ###   ########.fr       */
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

void HttpResponse::setGetHeader(std::string sessionId)
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
            << "\r\nset-cookie:" << sessionId
            << "\r\n\r\n";
    _header = bufferM.str(); 
}

void HttpResponse::setPostHeader(std::string sessionId)
{
    std::ostringstream bufferM;
    if (_status == 403)
    {
        bufferM << "HTTP/1.1 " << _status << " Forbidden"
                << "\r\nContent-type: " << _contentType
                << "\r\nServer: " << _host
                << "\r\nContent-length: " << _contentLength
                << "\r\nset-cookie:" << sessionId
                << "\r\n\r" << std::endl;
    }
    else
    {
        bufferM << "HTTP/1.1 " << _status
                << "\r\ncontent-type: " << _contentType
                << "\r\nserver: " << _host
                << "\r\ncontent-length: " << _contentLength
                << "\r\nset-cookie:" << sessionId
                << "\r\n\r" << std::endl;
        
    }
    _header = bufferM.str(); 
}

void HttpResponse::setDeleteHeader(std::string sessionId)
{
    std::ostringstream bufferM;
    bufferM << "HTTP/1.1" << _status
            << "\r\ncontent-type: " << _contentType
            << "\r\nserver:" << _host
            << "\r\ncontent-length: " << _contentLength
            << "\r\nset-cookie:" << sessionId
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

void HttpResponse::writeContent(std::string path, t_info  &info, HttpRequest &request)
{   
    struct stat file;
    std::string full_path;
    if (path.find("/") != 0 && info._root.find_last_of("/") != info._root.size() - 1)
    { 
        full_path = info._root + "/";
        full_path += path;
        info._root += "/";
    }
    else
        full_path = info._root + path;
    full_path = full_path.substr(0, full_path.size() - 1 );
    if (stat(full_path.c_str(), &file) == 0)
    {
        if (S_ISREG(file.st_mode))
            path = path.substr(0, path.size() - 1);
    }
    
    if (path.find_last_of('/') != path.size() - 1)
    {
        try
        {
            if (info._cgiPath.empty())
                writeNormalPage(path, info);
            else

                writeCgiPage(path, info, request);
        }
        catch (HttpRequest::HttpPageNotFoundException &e)
        {
            writeErrorPage(info, 404);
        }
        catch (HttpRequest::HttpPageForbiddenException &e)
        {
            writeErrorPage(info, 403);
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
            writeErrorPage(info, 404);
        }
        catch (HttpRequest::HttpPageForbiddenException &e)
        {
            writeErrorPage(info, 403);
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

void HttpResponse::writeCgiPage(std::string path, t_info  &info, HttpRequest &request)
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
    if (info._cgiPath.find("ubuntu_cgi_tester"))
        pageCreate.solveCgiTester(path, info, content, request);
    else
        pageCreate.solveCgiPhp(path, info, content, request);

    //find type of response
    size_t h1 = content.find("Content-type: ");
    if (h1 == std::string::npos)
        h1 = content.find("Content-Type: ");
    size_t h2 = content.find("\r\n", h1 + 14);
    if (h2 == std::string::npos)
        h2 = content.find("\n", h1 + 14);
    std::string type = "*/*";
    if (h1 != std::string::npos)
    {
        std::string type = content.substr(h1 + 14 , h2 - h1 - 14);
    }
    //find status of response
    h1 = content.find("Status: ");
    h2 = content.find("\r\n", h1 + 8);
    if (h2 == std::string::npos)
        h2 = content.find("\n", h1 + 8);
    if (h1 != std::string::npos)
    {
        std::string status = content.substr(h1 + 8, h2 - h1 - 8);
        std::cout << status << std::endl;
        std::stringstream X(status);
        std::string line;
        getline(X, line, ' ');
        _status = strtol(line.c_str(), NULL, 10);
    }
    
    
    //write content
    size_t header_end = content.find("\r\n\r\n");
    if (header_end == std::string::npos)
    {
        header_end = content.find("\n\n", h2 + 1);
        if (header_end != std::string::npos)
            content = content.substr(header_end + 2, content.size() - 1 - header_end - 2);
    }
    else
        content = content.substr(header_end + 4, content.size() - 1 - header_end - 4);

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

void HttpResponse::writeErrorPage(t_info &info, int error)
{
    std::string content = "";
    std::string path;
    if (info._errorPages.find(error) != info._errorPages.end())
        path = info._errorPages.find(error)->second;
    else
    {
        _status = 500;
        writeFailError();
        return;
    }
    _status = error;
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

void HttpResponse::setGetRedirectHeader(t_info &Info, std::string sessionId)
{
    std::istringstream ss(Info._redirect);
    std::string addr;
    ss >> addr >> addr;
    std::ostringstream bufferM;
    if (_status == 301)
    {
        bufferM << "HTTP/1.1 " << _status << " Moved Permanently"
                << "\r\ncontent-type: " << "text/html"
                << "\r\nLocation: " << "http://" << addr
                << "\r\ncontent-length: " << 0
                << "\r\nset-cookie:" << sessionId
                << "\r\n\r\n";
    }
    else
    {
        bufferM << "HTTP/1.1 " << _status << " Found"
                << "\r\ncontent-type: " << "text/html"
                << "\r\nLocation: " << "http://" << addr
                << "\r\ncontent-length: " << 0
                << "\r\nset-cookie:" << sessionId
                << "\r\n\r\n";
    }
    _header = bufferM.str(); 
}
