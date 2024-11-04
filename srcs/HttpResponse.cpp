/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:37 by masoares          #+#    #+#             */
/*   Updated: 2024/11/04 11:03:36 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "HttpResponse.hpp"

HttpResponse::HttpResponse(int client , Server *server)
{
    _client_fd = client;
    _host = server->getHost();
    _port = server->getPorts();


}

HttpResponse::~HttpResponse()
{}


void HttpResponse::setGetHeader()
{
    std::string result;
    result = "";
    if(_status == 200)
        result = "OK";
    std::ostringstream bufferM;
    bufferM << "HTTP/1.1" << _status << result
            << "\r\ncontent-type: " << _contentType
            << "\r\nserver:" << _host
            << "\r\ncontent-length: " << _contentLength
            << "\r\n\r\n";
    _header = bufferM.str(); 
}

void HttpResponse::setPostHeader()
{
    std::ostringstream bufferM;
    bufferM << "HTTP/1.1" << _status
            << "\r\ncontent-type: " << _contentType
            << "\r\nserver:" << _host << ":" << _port
            << "\r\ncontent-length: " << _contentLength
            << "\r\n\r\n";
    _header = bufferM.str(); 
}

void HttpResponse::setDeleteHeader()
{
    std::ostringstream bufferM;
    bufferM << "HTTP/1.1" << _status
            << "\r\ncontent-type: " << _contentType
            << "\r\nserver:" << _host << ":" << _port
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

void HttpResponse::writeContent(std::string path, Server *server)
{
    char buffer[100];
    std::string content = "";
    int fd;
    
    if (path.find_last_of('/') != path.size() - 1)
    {
        try
        {
            path = server->getRoot() + path;
            
            memset(buffer, 0, 100);
            
            std::fstream file;
            file.open(path.c_str());
            if (!file.is_open())
                throw(std::exception());
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
            path = server->getRoot() + server->getErrorPages().find(404)->second;
            _status = 404;
            fd = open(path.c_str(), O_RDONLY);
            if (fd == -1)
            {
                _status = 500;
                throw(std::exception()); //error 500 escrito na string
            }
            memset(buffer, 0, 100);
            while (read(fd, buffer, 99))
            {
                content = content + buffer;
                memset(buffer, 0, 100);
            }
            std::ostringstream bufferM;
            std::string output = bufferM.str();
            setContent(content);
            setLength(content.size());
        }
        catch (std::exception &e)
        {
            path = server->getErrorPages().find(403)->second;
            fd = open(path.c_str(), O_RDONLY);
            if (fd == -1)
            {
                _status = 500;
                throw(std::exception()); //error 500 escrito na string
            }
            memset(buffer, 0, 100);
            while (read(fd, buffer, 99))
            {
                content = content + buffer;
                memset(buffer, 0, 100);
            }
            setContent(content);
            setLength(content.size());
        }
    }
    else
    {
        try
        {
            if (server->getIndex().empty())
            {
                path = server->getRoot() + "index.html";
                fd = open(path.c_str(), O_RDONLY);
                if (fd == -1)
                {
                    if (errno == ENOENT)
                        throw(HttpRequest::HttpPageNotFoundException());
                    else if (errno == EACCES)
                        throw(std::exception());
                }
            }
            else
            {
                for (size_t i = 0; i < server->getIndex().size(); i++)
                {
                    path = server->getRoot() + "/" + server->getIndex()[i];
                    fd = open(path.c_str(), O_RDONLY);
                    if (fd != -1)
                        break;
                }
                if (fd == -1)
                {
                    throw(std::exception());
                }
            }
            size_t bytes_read;
            memset(buffer, 0, 100);
            while ((bytes_read = read(fd, buffer, 99)) > 0)
            {
                content = content + buffer;
                memset(buffer, 0, 100);
            }
            setContent(content);
            setLength(content.size());
        }
        catch (std::exception &e)
        {
            
        }
    }
    
}


void HttpResponse::handleDataUpload(std::string path, HttpRequest &request, Server *server)
{
    if (path.find_last_of('/') != path.size() - 1)
    {
        std::string filename = server->getRoot() + path;
        int fd = open(path.c_str(), O_RDWR|O_CREAT);
        write(fd, (request.getRequest()).c_str(), request.getRequest().size());
        close(fd);
        
    }
    else //if it is a folder
    {
        //check if path exists
        path = server->getRoot() + path;
        DIR *dir = opendir(path.c_str());
        if (dir == NULL){
            throw(std::exception());
        }
        closedir(dir);
        if (request.searchProperty("Content-Type").find("multipart/form-data") == std::string::npos)
        {
            //creates a file with the date in the path
            time_t timestamp;
            time(&timestamp);
            std::string filename = ctime(&timestamp);
            std::replace(filename.begin(), filename.end(), ' ', '_');
            std::replace(filename.begin(), filename.end(), ':', '-');
            path = path + filename;
            
            int fd = open(path.c_str(), O_RDWR|O_CREAT);
            if (fd == -1)
                throw(std::exception());
            write(fd, (request.getRequest()).c_str(), request.getRequest().size());
            close(fd);
        }
        else
        {
            //find the boundary
            size_t startline = request.getRequest().find("boundary=") + 9;
            size_t endline = request.getRequest().find("\r\n", startline);
            std::string boundary = request.getRequest().substr(startline, endline - startline - 1);
            
            size_t pos = request.getRequest().find("\r\n");

            //check if file exists
            size_t header_advance = request.getRequest().find("\r\n\r\n", pos);
            if (header_advance == std::string::npos)
            {
                return;
            }
            std::string header = request.getRequest().substr(pos, header_advance - pos + 4);
            std::string filename = server->getRoot() + "/" + getFilenameUploaded(header);
            int i = 1;
            struct stat buffer;
            while (stat(filename.c_str(), &buffer) == 0)
            { 
                std::stringstream X;
                X << i;
                std::string num = X.str();
                filename = server->getRoot() + "/(" + num + ")" + getFilenameUploaded(header);
                i++;
            }
            
            while (pos != std::string::npos)
            {
                //find next boundary (chunk)
                pos = pos + boundary.size();
                size_t header_advance = request.getRequest().find("\r\n\r\n", pos);
                if (header_advance == std::string::npos)
                {
                    return;
                }
                pos = header_advance + 4;
                size_t part_end = request.getRequest().find(boundary, pos);
                if (part_end == std::string::npos)
                    break;

                //write content to file
                std::string file_content = request.getRequest().substr(pos, part_end - pos);
                std::ofstream file;
                file.open(filename.c_str(), std::ios::app);
                if (!file.is_open()) 
                {
                    throw(std::exception());
                }
                file << file_content;
                file.close();
                pos = request.getRequest().find(boundary, pos);
            }
        }
    }    
}


std::string HttpResponse::getFilenameUploaded(std::string header)
{
    std::string filename;
    size_t pos = header.find("filename=\"") + 10;
    size_t final_pos = header.find("\"", pos);
    filename = header.substr(pos, final_pos - pos);
    return filename;
    
}


void HttpResponse::handleDataDeletion(std::string path, HttpRequest &request, Server *server)
{
    (void) request;
    if (path.find_last_of('/') != path.size() - 1)
    {
        std::string filename = server->getRoot() + path;
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
        std::string folder = server->getRoot() + path;
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