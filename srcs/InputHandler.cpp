/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 23:51:55 by masoares          #+#    #+#             */
/*   Updated: 2024/12/12 19:40:40 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "InputHandler.hpp"

InputHandler::InputHandler()
{}

InputHandler::~InputHandler()
{}

void InputHandler::handleDataUpload(std::string path, HttpRequest &request, t_info &info, HttpResponse &response)
{
    
    if (!info._cgiPath.empty())
    {
        writePostCgiPage(path, info, request, response);   
        return;
    }
    int pathExistance = definePathType(path, info);
    
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
            fileSaver(request, info, path);
        }
    }
    info._status = 204;
}

void InputHandler::writePostCgiPage(std::string path, t_info  &info, HttpRequest &request, HttpResponse &response)
{
    std::string content = "";
    struct stat test;
    std::fstream file;
    if (path.find("/") != 0 && info._root.find_last_of("/") != info._root.size() - 1)
    { 
        info._root += "/";
    }
    path = info._root + path;
    std::string full_path = path;
    //full_path = full_path.substr(0, full_path.size() - 1 );
    if (stat(full_path.c_str(), &test) == 0)
    {
        if (S_ISREG(test.st_mode) && path.find_last_of("/") == path.size() -1 )
            path = path.substr(0, path.size() - 1);
    }
    else if (path.find_last_of("/") == path.size() - 1)
        path = path.substr(0, path.size() - 1);
    file.open(path.c_str());
    if (!file.is_open())
    {
        file.open(path.c_str(), std::ios::out);
        if (!file.is_open())
            throw(HttpRequest::HttpPageNotFoundException());
    }
        
    file.close();
    CgiManagement pageCreate;
    // if (info._cgiPath.find("ubuntu_cgi_tester"))
    pageCreate.solveCgiTester(path, info, content, request);
    // else
    //     pageCreate.solveCgiPhp(path, info, content, request);
    
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
        type = content.substr(h1 + 14 , h2 - h1 - 14);
        std::cout << type << std::endl;
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
        info._status = strtol(line.c_str(), NULL, 10);
    }

    //write content
    size_t header_end = content.find("\r\n\r\n");
    if (header_end == std::string::npos)
    {
        header_end = content.find("\n\n", h2 + 1);
        if (header_end != std::string::npos)
            content = content.substr(header_end + 2, content.size() - header_end - 2);
    }
    else
        content = content.substr(header_end + 4, content.size() - header_end - 4);


    response.setContent(content);
    response.setContentType(type);
}


int InputHandler::definePathType(std::string &path, t_info &info)
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

void InputHandler::writeToFilename(HttpRequest request, std::string filename)
{
    std::string file_content = request.getRequestBody();
    std::ofstream file;
    
    file.open(filename.c_str(), std::ios::app);
    if (!file.is_open()) 
    {
        throw(std::exception());
    }
    file << file_content;
    file.close();
}

void InputHandler::logFileCreation(std::string &path, HttpRequest &request)
{
    time_t timestamp;
    time(&timestamp);
    std::string filename = ctime(&timestamp);
    std::replace(filename.begin(), filename.end(), ' ', '_');
    std::replace(filename.begin(), filename.end(), ':', '-');
    path = path + filename;
    

    std::string file_content = request.getRequestBody();
    std::ofstream file;
    
    file.open(path.c_str(), std::ios::app);
    if (!file.is_open()) 
    {
        throw(std::exception());
    }
    file << file_content;
    file.close();
}

void InputHandler::fileSaver(HttpRequest &request, t_info &info, std::string path)
{
    //find the boundary 
    size_t startline = request.getHeader().find("boundary=") + 9;
    size_t endline = request.getHeader().find("\r\n", startline);
    std::string boundary = request.getHeader().substr(startline, endline - startline - 1);

    //check if file has at least one part
    size_t header_advance = request.getRequestBody().find("\r\n\r\n", 0);
    if (header_advance == std::string::npos)
    {
        info._status = 400;
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
    
        std::string filename = path + "/" + originalFilename;
        
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

std::string InputHandler::getFilenameUploaded(std::string header)
{
        std::string filename;
    size_t pos = header.find("filename=\"") + 10;
    size_t final_pos = header.find("\"", pos);
    
    filename = header.substr(pos, final_pos - pos);
    
    return filename;
}

std::string InputHandler::getNameUpload(std::string header)
{
    std::string name;
    size_t pos = header.find("name=\"") + 6;
    size_t final_pos = header.find("\"", pos);
    name = header.substr(pos, final_pos - pos);

    return name;
}


void InputHandler::addToFile(size_t &pos, size_t &part_end, std::string filename, HttpRequest &request, std::string boundary)
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
