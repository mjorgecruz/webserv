/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   general.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-ffe <luis-ffe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 14:42:35 by masoares          #+#    #+#             */
/*   Updated: 2024/11/17 10:54:06 by luis-ffe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef GENERAL_HPP
# define GENERAL_HPP

extern int g_signal;

#include "webserv.hpp"
#include "Socket.hpp"
#include "Server.hpp"
#include "InputHandler.hpp"
#include "DeleteHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Http.hpp"
#include "Location.hpp"
#include "SessionManagement.hpp"

bool isValidError(int errorCode);
bool isNumeric(const std::string &str);
void custtomThrow(std::string str);
#endif

