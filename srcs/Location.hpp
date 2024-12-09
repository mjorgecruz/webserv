/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 13:49:56 by masoares          #+#    #+#             */
/*   Updated: 2024/12/09 09:22:14 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "Socket.hpp"
#include "Server.hpp"

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
        long _maxBodySize;
        std::string _authFile;
        
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
        std::string getCgiPath();
        std::vector<std::string> getAllowedMethods();
        std::map<int, std::string> getErrorPages();
        std::string getRedirect();
        int getAutoIndex();
        long getMaxBodySize();
        
        void parseLocation(std::string &line, std::ifstream &file);
        void printLocationConfig() const;
        
        void keywordIndex(std::istringstream &iss);
        void keywordRoot(std::istringstream &iss);
        void keywordCgiPath(std::istringstream &iss);
        void keywordMethods(std::istringstream &iss);
        void keywordErrorPages(std::istringstream &iss);
        void keywordReturn(std::istringstream &iss);
        void keywordMaxBodySize(std::istringstream &iss);

    class exceptionAtLocation : public std::exception
	{
	    public:
		    virtual const char *what() const throw();
	};

};

