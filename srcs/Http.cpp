/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 13:37:26 by masoares          #+#    #+#             */
/*   Updated: 2024/10/29 10:12:10 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Http.hpp"

Http::Http( void )
{
	int epoll_fd = epoll_create(1);

	if (epoll_fd == -1) {
        throw(std::exception());
	}
}

Http::Http( Configs const &config)
{
	_epollFd = epoll_create(1);

	if (_epollFd == -1) {
        throw(std::exception());
	}
    addServersToList(config);
}

Http::~Http( void )
{
    
}


void Http::addEpollServer( Server &server )
{
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP;
	event.data.fd = server.getServerFd();
    
    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, event.data.fd, &event) == -1)
    {
        throw(std::exception());
    }
}

void Http::addServersToList(Configs const & config)
{
    size_t size = config.configSize();
    for (size_t i = 0; i < size; i++)
    {
        Server *server = new Server(config[i]->ports, config[i]->host);
        _listServers.push_back(server);
        addEpollServer( *server );
    }
}

Server *Http::operator[](int num)
{
    int len = this->_listServers.size();
    if (num >= len)
        throw(std::exception());
    return this->_listServers[num];
}

int Http::listServersSize() const
{
    return this->_listServers.size();
}

void Http::runApplication()
{
    struct epoll_event events[MAX_EVENTS];
    int event_count;

    while (g_signal == 0)
    {
        event_count = epoll_wait(_epollFd, events, MAX_EVENTS, 30000);
        
        for (int i = 0; i < event_count; i++) 
        {
            if (events[i].events  & (EPOLLRDHUP | EPOLLERR | EPOLLHUP))
            {
                std::cout << "Page was hard refreshed" << std::endl;
                close(events[i].data.fd);
            }
            //else if(events[i].data.fd == server_socket)
            //{
            //    accept_new_connection(server_socket, _epollFd);
            //}
            else if (events[i].events & (EPOLLIN | EPOLLET ))
            {
                read_data_from_socket(events[i].data.fd);
                close(events[i].data.fd);
            }
            else
                accept_new_connection(events[i].data.fd, _epollFd);
		}
    }
    
	if (close(_epollFd)) {
		fprintf(stderr, "Failed to close epoll file descriptor\n");
		return;
	}
    for (size_t i = 0; i < _listServers.size(); i++)
    {
        close(_listServers[i]->getServerFd());
        delete(_listServers[i]);
    }
}