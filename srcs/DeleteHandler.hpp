/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 12:01:32 by masoares          #+#    #+#             */
/*   Updated: 2024/11/16 14:32:33 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#pragma once

#include "general.hpp"

class Server;
class HttpRequest;
class Location;
class HttpResponse;

struct t_info_struct;
typedef struct s_info t_info;

class DeleteHandler
{
    public:
        DeleteHandler();
        ~DeleteHandler();

        void handleDataDeletion(std::string path, HttpRequest &request, t_info &info);
        int definePathType(std::string &path, t_info &info);

        class DeleteFileForbiddenException: public std::exception
        {
            public:
                const char *what( void ) const throw();
        };
        class DeleteFileMissingException: public std::exception
        {
            public:
                const char *what( void ) const throw();
        };
};
