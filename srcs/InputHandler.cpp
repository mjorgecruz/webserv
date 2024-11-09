/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 23:51:55 by masoares          #+#    #+#             */
/*   Updated: 2024/11/09 00:00:17 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "InputHandler.hpp"

void InputHandler::handleDataUpload(std::string path, HttpRequest &request, t_info &info)
{
    
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
            fileSaver(request, server, path);
        }
    }
    _status = 204; 
}

int definePathType(std::string &path, t_info &info)
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

std::string InputHandler::getFilenameUploaded(std::string header);
std::string InputHandler::getNameUpload(std::string header);


void InputHandler::logFileCreation(std::string &path, HttpRequest &request);
void InputHandler::fileSaver(HttpRequest &request, Server *server, std::string path);
void InputHandler::addToFile(size_t &pos, size_t &part_end, std::string filename, HttpRequest &request, std::string boundary);
