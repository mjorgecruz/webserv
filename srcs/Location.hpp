/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-ffe <luis-ffe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 13:49:56 by masoares          #+#    #+#             */
/*   Updated: 2024/10/30 09:54:03 by luis-ffe         ###   ########.fr       */
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
        
    public:
        Location( void );
        ~Location( void );
        void addIndex(std::string index);
        void setRoot(std::string root);
        void addAllowedMethods(std::string method);
        void addErrorPages(int errorNum, std::string error);
        void setRedirect(std::string redir);
        void setAutoIndex(bool autoindex);
        
        std::vector<std::string> getIndex();
        std::string getRoot();
        std::vector<std::string> getAllowedMethods();
        std::map<int, std::string> getErrorPages();
        std::string getRedirect();
        bool getAutoIndex();

        //getters setters
    
};

