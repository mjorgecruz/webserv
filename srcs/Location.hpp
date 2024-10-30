/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 13:49:56 by masoares          #+#    #+#             */
/*   Updated: 2024/10/30 08:59:02 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#pragma once

#include "general.hpp"

class Location
{
    private:
        std::vector<std::string> _index;
        std::string _root;
        std::vector<std::string> _allowedMethods;
        std::map<int, std::string> _errorPages;
        std::string _redirect;
        bool _autoIndex;
        
    public:
        //getters setters
    
};

