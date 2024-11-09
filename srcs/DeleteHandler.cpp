/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 12:00:58 by masoares          #+#    #+#             */
/*   Updated: 2024/11/09 12:15:16 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DeleteHandler.hpp"

DeleteHandler::DeleteHandler()
{}

DeleteHandler::~DeleteHandler()
{}

void DeleteHandler::handleDataDeletion(std::string path, HttpRequest &request, t_info &info)
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
        info._status = 204;
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
        info._status = 204;
    }
}

int DeleteHandler::definePathType(std::string &path, t_info &info)
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