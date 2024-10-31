/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-ffe <luis-ffe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 13:37:26 by masoares          #+#    #+#             */
/*   Updated: 2024/10/30 15:02:46 by luis-ffe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http.hpp"
#include <fstream>

Http::Http( void )
{
	int epoll_fd = epoll_create(1);

	if (epoll_fd == -1) {
        throw(std::exception());
	}
    _epollFd = epoll_fd;
}

void Http::webservInitializer(std::string confPath)
{
    std::ifstream file(confPath);
    if(!file)
    {
        std::cout << "Could Not Open :" << confPath << std::endl;
        return ;
    }
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;
        else
        {
            Server *server = new Server();
            try
            {
                server->serverChecker(line, file);    
            }
            catch (std::exception &e)
            {
                delete server;
                std::cout << "Server Creation Error in ServerChecker @ WebservInitializer" << std::endl;
            }
            
            if (server)
                addServerToList(server);
            else
            {
                std::cout << "Server Creation Error in ServerChecker @ WebservInitializer" << std::endl;
                throw(std::exception());
            }
        }
    }
    file.close();


    for (size_t i = 0; i < _listServers.size(); i++)
    {
        _listServers[i]->createSocket(_listServers[i]->getPorts(), _listServers[i]->getHost());
    }
    this->addServersToEpoll();  
}

void Http::addServerToList(Server *server)
{
    _listServers.push_back(server);
}


Http::~Http( void )
{
    close(_epollFd);
    for (size_t i = 0; i < _listServers.size(); i++)
    {
        close(_listServers[i]->getSocketFd());
        delete(_listServers[i]);
    }
}


void Http::addEpollServer( Server *server )
{
    struct epoll_event event;
    //memset()
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP;
	event.data.fd = server->getSocketFd();
    
    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, server->getSocketFd(), &event) == -1)
    {
        throw(std::exception());
    }
}

void Http::addServersToEpoll( void )
{
    size_t size = _listServers.size();
    for (size_t i = 0; i < size; i++)
    {
        addEpollServer(_listServers[i]);
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
            int added = 0;
            for (size_t j = 0; j < this->_listServers.size(); j++ )
            {
                if(events[i].data.fd == this->_listServers[j]->getSocketFd())
                {
                    accept_new_connection(this->_listServers[j]->getSocketFd(), _epollFd);
                    added = 1;
                    break;
                }
            }
            if (events[i].events  & (EPOLLRDHUP | EPOLLERR | EPOLLHUP))
            {
                std::cout << "Page was hard refreshed" << std::endl;
                close(events[i].data.fd);
            }
            // else if(events[i].data.fd == this->_listServers[i]->getServerFd())
            // {
            //    accept_new_connection(this->_listServers[i]->getServerFd(), _epollFd);
            // }
            else if (added == 0 && events[i].events & (EPOLLIN | EPOLLET ))
            {
                read_data_from_socket(events[i].data.fd);
                close(events[i].data.fd);
            }
            // else
            //     accept_new_connection(events[i].data.fd, _epollFd);
		}
    }
    
	if (close(_epollFd)) {
		fprintf(stderr, "Failed to close epoll file descriptor\n");
		return;
	}
}
