/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-ffe <luis-ffe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 13:49:56 by masoares          #+#    #+#             */
/*   Updated: 2024/11/01 10:03:23 by luis-ffe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "general.hpp"

class Location
{
    private:
        std::vector<std::string> _index;
        std::string _root;
        std::string _cgiPath;
        std::vector<std::string> _allowedMethods;
        std::map<int, std::string> _errorPages;
        std::string _redirect;
        bool _autoIndex;
        std::string _path;
        
    public:
        Location( void );
        ~Location( void );
        void addIndex(std::string index);
        void setRoot(std::string root);
        void setCgiPath(std::string cgiPath);
        void addAllowedMethods(std::string method);
        void addErrorPages(int errorNum, std::string error);
        void setRedirect(std::string redir);
        void setAutoIndex(bool autoindex);
        
        std::string getPath();
        std::vector<std::string> getIndex();
        std::string getRoot();
        std::vector<std::string> getAllowedMethods();
        std::map<int, std::string> getErrorPages();
        std::string getRedirect();
        bool getAutoIndex();
        
        void parseLocation(std::string &line, std::ifstream &file);
        void printLocationConfig() const;
        //getters setters
};

