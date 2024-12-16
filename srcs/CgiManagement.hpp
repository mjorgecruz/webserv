/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiManagement.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 17:58:52 by masoares          #+#    #+#             */
/*   Updated: 2024/12/14 12:39:33 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "InputHandler.hpp"
#include "HttpRequest.hpp"
#include <unistd.h>
#include <sys/wait.h>

struct t_info_struct;
typedef struct s_info t_info;

class CgiManagement
{

    public:
        CgiManagement();
        ~CgiManagement();
        

        void solveCgiTester(std::string file, t_info &info, std::string &content, HttpRequest &request);
        void getCgiTester(std::string requ, std::string file, t_info &info, std::string &content, HttpRequest &request);
        void postCgiTester(std::string requ, std::string file, t_info &info, std::string &content, HttpRequest &request);
        

};