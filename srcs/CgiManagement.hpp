/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiManagement.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 17:58:52 by masoares          #+#    #+#             */
/*   Updated: 2024/11/17 01:42:17 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "general.hpp"

class CgiManagement
{

    public:
        CgiManagement();
        ~CgiManagement();
        
        void solveCgiPhp(std::string file, t_info &info, std::string &content);
        
        
    
};