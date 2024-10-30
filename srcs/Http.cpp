/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 13:37:26 by masoares          #+#    #+#             */
/*   Updated: 2024/10/30 16:11:47 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Http.hpp"

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
    //ler doc
    //criar n servers com n locations e devidos campos populados
    //criar de forma dinamica

    /*
    while (not reached EOF(confPath))
    {
        IF() each line checks keyords 
            when sever detected enters server creation logic and adds to list (location logic and parsing in this scope aswell)
                advances line by line until it closes the server brackets
                
        goes to next line if not yet at EOF and (keeps looping)
    }

    how to catch erros ? default path for cgi 2x and defaul.conf file paramethers ?   
    */
    (void) confPath;
    Server *test = new (Server);
    test->setConfigs("");
    this->addServerToList(test);
    
    
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

void Http::accept_new_connection(int server_socket, int epoll_fd )
{
    int client_fd;
    sockaddr client;
    socklen_t length = 0;
    struct epoll_event event;

    memset(&client, 0, sizeof(client));
    client_fd = accept(server_socket, &client, &length);
    if (client_fd == -1)
    {
        std::cout<< "fd negativo" << std::endl;
        return;
    }
    event.data.fd = client_fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
    std::cout<< "Added: " << client_fd << std::endl;
}

void Http::read_data_from_socket(int socket)
{
    struct sockaddr_in addr;
    socklen_t len;
    int serverNumber = 0;
    int server_fd = 0;

    //check the corresponding server
    if (getsockname(socket, (sockaddr *) &addr, &len) == -1)
        throw(std::exception());
    int port = ntohs(addr.sin_port);
    std::string address = inet_ntoa(addr.sin_addr);
    std::cout << "ADDRESS : " << address << std::endl;
    std::cout << "PORT : " << port << std::endl;
    while (serverNumber < _listServers.size())
    {
        if (port == _listServers[serverNumber]->getPorts())
        {
            if (address == _listServers[serverNumber]->getHost())
            {
                server_fd = _listServers[serverNumber]->getSocketFd();
                break;
            }    
        }
    }
    
    std::string remainder = "";
    char buffer[BUFSIZ];
    int bytes_read;
    
    while (1)
    {    
        memset(&buffer, 0, sizeof buffer);
        bytes_read = recv(socket, buffer, BUFSIZ, 0);
        if (bytes_read < 0)
        {
            break;
        }
        else
        {
            std::string input(buffer, buffer + bytes_read);
            input = remainder + input;
            remainder = input;
            if (bytes_read < BUFSIZ)
                break;
        }
    }
    remainder = remainder + "\0";
    _request.setRequest(remainder);
    _request.fillReqProperties();
    _request.defineMimeType();
    _request.setClientFd(server_fd);

    reply(socket, _request);
}


void Http::analyzeRequest(Server *server, int socket)
{
    std::string type;
    std::string path;
    std::string httpVersion;
    std::istringstream request(_request.getRequestType());
    request >> type >> path >> httpVersion;
    std::map<std::string, Location *> possibleLocations = server->getLocations();
    size_t locationNum = 0;
    
    //check version
    if (httpVersion != "HTTP/1.1")
        throw(std::exception());
        
    //define location
    std::map<std::string, Location *>::iterator it = possibleLocations.begin();
    while (it != possibleLocations.end()) 
    {
        if (*(path.cend() -1) == '/')
        {
            DIR * root;
            root = opendir(((it->second)->getRoot() + path).c_str());
            if (root != NULL)
            {
                
            }
            if (it->second->getAutoIndex())
            {
                dirent picas = readdir(root);
                
            }
            
        }    
            //it is a path to directory
        locationNum++;
    }
    
    //check method
    

    //define connection type


    //searchfile

    //prepare reply
}
