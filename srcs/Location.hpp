/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-ffe <luis-ffe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 13:49:56 by masoares          #+#    #+#             */
/*   Updated: 2024/11/04 14:01:29 by luis-ffe         ###   ########.fr       */
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
        int _autoIndex;
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
        void setAutoIndex(int autoindex);
        
        std::string getPath();
        std::vector<std::string> getIndex();
        std::string getRoot();
        std::vector<std::string> getAllowedMethods();
        std::map<int, std::string> getErrorPages();
        std::string getRedirect();
        int getAutoIndex();
        
        void parseLocation(std::string &line, std::ifstream &file);
        void printLocationConfig() const;
        
        void keywordIndex(std::istringstream &iss);
        void keywordRoot(std::istringstream &iss);
        void keywordCgiPath(std::istringstream &iss);
        void keywordMethods(std::istringstream &iss);
        void keywordErrorPages(std::istringstream &iss);
        void keywordReturn(std::istringstream &iss);
};

