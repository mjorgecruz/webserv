/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 23:32:41 by masoares          #+#    #+#             */
/*   Updated: 2024/11/08 19:33:36 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "general.hpp"

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
    
    std::string _cgiPath;
    std::string _redirect;
    std::string _path;

}   t_info;


class InputHandler
{
    public:
        void handleDataUpload(std::string path, HttpRequest &request, Server *server);
        std::string getFilenameUploaded(std::string header);
        std::string getNameUpload(std::string header);
        int definePathType(std::string &path, Server *server);
        void logFileCreation(std::string &path, HttpRequest &request);
        void fileSaver(HttpRequest &request, Server *server, std::string path);
        void addToFile(size_t &pos, size_t &part_end, std::string filename, HttpRequest &request, std::string boundary); 
};