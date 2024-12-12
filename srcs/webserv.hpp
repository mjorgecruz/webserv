/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 09:46:35 by masoares          #+#    #+#             */
/*   Updated: 2024/12/11 12:12:03 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <sstream>
#include <vector>
#include <map>
#include <csignal>
#include <algorithm>
#include <cstdlib>
#include <sys/types.h>
#include <dirent.h>
#include <ctime>
#include <fstream>
#include <sys/stat.h>
#include <cstdio>

bool isValidError(int errorCode);
bool isNumeric(const std::string &str);
void custtomServerThrow(std::string str);
void custtomLocationThrow(std::string str);
void handle_alarm(int sig);

#define PORT 8080
#define MAX_EVENTS 128



#endif