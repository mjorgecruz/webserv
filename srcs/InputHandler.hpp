/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 23:32:41 by masoares          #+#    #+#             */
/*   Updated: 2024/12/09 20:30:36 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "Socket.hpp"
#include "Server.hpp"
#include "Location.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"


class Server;
class HttpRequest;
class Location;
class HttpResponse;

typedef struct s_info
{
    std::string _host;
    int _ports;
    std::string _root;
    std::vector<std::string> _hostname;
    std::vector<std::string> _index;
    std::map<int, std::string> _errorPages;
    std::map<std::string, Location *> _locations;
    std::vector<std::string> _allowedMethods;
    long _maxBodySize;
    int _autoIndex;
    int _status;
    
    std::string _cgiPath;
    std::string _redirect;
    std::string _path;
    std::string _authFile;
    
    void printInfoConfig() const;

}   t_info;


class InputHandler
{
    public:
        InputHandler();
        ~InputHandler();

        
        void handleDataUpload(std::string path, HttpRequest &request, t_info &info, HttpResponse &response);
        
        void writePostCgiPage(std::string path, t_info  &info, HttpRequest &request, HttpResponse &response);
        
        int definePathType(std::string &path, t_info &info);
        void writeToFilename(HttpRequest request, std::string filename);
        void logFileCreation(std::string &path, HttpRequest &request);
        void fileSaver(HttpRequest &request, t_info &info, std::string path);
        std::string getFilenameUploaded(std::string header);
        std::string getNameUpload(std::string header);
        void addToFile(size_t &pos, size_t &part_end, std::string filename, HttpRequest &request, std::string boundary);
};