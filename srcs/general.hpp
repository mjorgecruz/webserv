/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   general.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 14:42:35 by masoares          #+#    #+#             */
/*   Updated: 2024/12/08 02:45:48 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef GENERAL_HPP
# define GENERAL_HPP

extern int g_signal;

#include "webserv.hpp"
#include "Socket.hpp"
#include "Server.hpp"
#include "Location.hpp"
#include "InputHandler.hpp"
#include "DeleteHandler.hpp"
#include "HttpRequest.hpp"
#include "SessionManagement.hpp"
#include "HttpResponse.hpp"
#include "Http.hpp"
#include "CgiManagement.hpp"
#include "InputHandler.hpp"


bool isValidError(int errorCode);
bool isNumeric(const std::string &str);
void custtomServerThrow(std::string str);
void custtomLocationThrow(std::string str);
#endif

