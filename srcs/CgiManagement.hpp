/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiManagement.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 17:58:52 by masoares          #+#    #+#             */
/*   Updated: 2024/11/28 08:56:57 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "general.hpp"
#include <unistd.h>
#include <sys/wait.h>
class CgiManagement
{

    public:
        CgiManagement();
        ~CgiManagement();
        
        void solveCgiPhp(std::string file, t_info &info, std::string &content, HttpRequest &request);

};