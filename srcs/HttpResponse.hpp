/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:40:44 by masoares          #+#    #+#             */
/*   Updated: 2024/10/27 19:07:17 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "webserv.hpp"
#include <map>

class HttpResponse
{
    private:
        std::string output;
        std::vector<char> content;
        
    public:
        HttpResponse();
        ~HttpResponse();
        void setOutput();
        void setContent();
        std::string getOutput();
        std::string getContent();
};


#endif