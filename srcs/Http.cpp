/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 13:37:26 by masoares          #+#    #+#             */
/*   Updated: 2024/10/28 15:31:42 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Http.hpp"

Http::Http( void )
{
    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];
	int epoll_fd = epoll_create(1);

	if (epoll_fd == -1) {
        throw(std::exception());
	}
}

Http::Http( Configs const &config)
{
    size_t size = config.configSize();
    for (int i = 0; i < size; i++)
    {
        
    }
}

Http::~Http( void )
{
    
}


void Http::addEpollServer( Server server )
{
    int event_count;
    int i;
    
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP;
	event.data.fd = server.getServerFd();
    
    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, event.data.fd, &event) == -1)
    {
        throw(std::exception());
    }
}