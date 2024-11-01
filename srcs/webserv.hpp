/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 09:46:35 by masoares          #+#    #+#             */
/*   Updated: 2024/11/01 18:35:15 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

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

#define PORT 8080
#define MAX_EVENTS 128



#endif