/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:37 by masoares          #+#    #+#             */
/*   Updated: 2024/11/08 19:34:41 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
            << "\r\n\r\n";
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

void HttpResponse::writeContent(std::string path, Server *server)
{
    std::string content = "";
    std::fstream file;
    
    if (path.find_last_of('/') != path.size() - 1)
    {
        try
        {
            path = server->getRoot() + path;
            
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
            path = server->getErrorPages().find(404)->second;
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
            path = server->getErrorPages().find(403)->second;
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
            if (server->getIndex().empty())
            {
                std::fstream file;
                path = server->getRoot() + "index.html";
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
                for (size_t i = 0; i < server->getIndex().size(); i++)
                {
                    path = server->getRoot() + "/" + server->getIndex()[i];
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


void HttpResponse::handleDataUpload(std::string path, HttpRequest &request, Server *server)
{

    int pathExistance = definePathType(path, server);
    
    //check if path is folder or file
    if (path.find_last_of('/') != path.size() - 1)
    {
        writeToFilename(request, path);
    }
    else //if it is a folder
    {
        //check if path exists
        if (pathExistance < 0)
        {
            if (pathExistance == -1)
                throw(std::exception());
            else if (pathExistance == -2)
                throw(std::exception());
        }
        
        //check if POST will be sent in parts or not
        if (request.searchProperty("Content-Type").find("multipart/form-data") == std::string::npos)
        {
            //creates a file with the date in the path
            logFileCreation(path, request);
        }
        else
        {
            //save all files sent
            fileSaver(request, server, path);
        }
    }
    _status = 204;    
}


std::string HttpResponse::getFilenameUploaded(std::string header)
{
    std::string filename;
    size_t pos = header.find("filename=\"") + 10;
    size_t final_pos = header.find("\"", pos);
    
    filename = header.substr(pos, final_pos - pos);
    
    return filename;
}

std::string HttpResponse::getNameUpload(std::string header)
{
    std::string name;
    size_t pos = header.find("name=\"") + 6;
    size_t final_pos = header.find("\"", pos);
    name = header.substr(pos, final_pos - pos);

    return name;
}

int HttpResponse::definePathType(std::string &path, Server *server)
{
    //path will end in "/" if path is of a directory
    path = server->getRoot() + path;
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

void HttpResponse::logFileCreation(std::string &path, HttpRequest &request)
{
    time_t timestamp;
    time(&timestamp);
    std::string filename = ctime(&timestamp);
    std::replace(filename.begin(), filename.end(), ' ', '_');
    std::replace(filename.begin(), filename.end(), ':', '-');
    path = path + filename;
    
    int fd = open(path.c_str(), O_RDWR|O_CREAT);
    if (fd == -1)
        throw(std::exception());
    write(fd, (request.getRequestBody()).c_str(), request.getRequestBody().size());
    close(fd);
}

void HttpResponse::fileSaver(HttpRequest &request, Server *server, std::string path)
{
    //find the boundary 
    size_t startline = request.getHeader().find("boundary=") + 9;
    size_t endline = request.getHeader().find("\r\n", startline);
    std::string boundary = request.getHeader().substr(startline, endline - startline - 1);

    //check if file has at least one part
    size_t header_advance = request.getRequestBody().find("\r\n\r\n", 0);
    if (header_advance == std::string::npos)
    {
        _status = 400;
        throw(std::exception());
    }
    
    size_t pos = 0;
    
    while (pos != std::string::npos)
    {
        //define filename
        header_advance = request.getRequestBody().find("\r\n\r\n", pos);
        std::string part_header = request.getRequestBody().substr(pos, header_advance + 4 - pos);
        std::string originalFilename = getFilenameUploaded(part_header);
        std::string name = getNameUpload(part_header);
    
        std::string filename = server->getRoot() + "/" + originalFilename;
        
        int i = 1;
        struct stat buffer;
        while (stat(filename.c_str(), &buffer) == 0)
        { 
            std::stringstream X;
            X << i;
            std::string num = X.str();
            filename = path + "(" + num + ")" + getFilenameUploaded(part_header);
            i++;
        }
        
        //loop to add stuff to the filename (looped until a new filename or name is found)
        while (pos != std::string::npos)
        {
            pos = pos + boundary.size();
            header_advance = request.getRequestBody().find("\r\n\r\n", pos);
            if (header_advance == std::string::npos)
                return;
            std::string part_header = request.getRequestBody().substr(pos, header_advance + 4 - pos);
            std::string newFilename = getFilenameUploaded(part_header);
            std::string newName = getNameUpload(part_header);
            if (newFilename != originalFilename && newName != name)
                break;

             //find next boundary (chunk)
            if (header_advance == std::string::npos)
                return;
            pos = header_advance + 4;
            size_t part_end = request.getRequestBody().find(boundary, pos);
            if (part_end == std::string::npos)
                return;
    
            //write content to file
            addToFile(pos, part_end, filename, request, boundary);
        }
    }
}

void HttpResponse::addToFile(size_t &pos, size_t &part_end, std::string filename, HttpRequest &request, std::string boundary)
{
    std::string file_content = request.getRequestBody().substr(pos, part_end - pos);
    std::ofstream file;
    
    file.open(filename.c_str(), std::ios::app);
    if (!file.is_open()) 
    {
        throw(std::exception());
    }
    file << file_content;
    file.close();
    pos = request.getRequestBody().find(boundary, pos);
}


void HttpResponse::handleDataDeletion(std::string path, HttpRequest &request, Server *server)
{
    (void) request;
    int pathExistance = definePathType(path, server);
    if (pathExistance < 0)
    {
        if (pathExistance == -1)
            throw(std::exception());
        else if (pathExistance == -2)
            throw(std::exception());
        }

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


void HttpResponse::writeToFilename(HttpRequest request, std::string filename)
{
    int fd = open(filename.c_str(), O_RDWR|O_CREAT);
    if (fd < 0)
    {
        std::cout << "ERRNO: " << errno << std::endl;
        throw(std::exception());
    }
    _status = 204;
    write(fd, (request.getRequestBody()).c_str(), request.getRequestBody().size());
    close(fd);
}