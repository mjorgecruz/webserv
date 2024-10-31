/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 13:37:26 by masoares          #+#    #+#             */
/*   Updated: 2024/10/31 17:30:04 by masoares         ###   ########.fr       */
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
                data_transfer(events[i].data.fd);
                // close(events[i].data.fd);
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

void Http::data_transfer(int socket)
{
    HttpRequest *request = new HttpRequest();

    int server_fd = 0;

    //check for the corresponding server
    Server *correspondingServer = findCorrespondingServer(socket); 
    server_fd = correspondingServer->getSocketFd();

    //create a struct with all info


    //fill request properties
    request->completeRequest(socket);
    request->setClientFd(server_fd);
    
    //prepare response
    // HttpResponse *response = new HttpResponse(socket, correspondingServer);

    // this->reply(socket, request, response, correspondingServer);


}

Server *Http::findCorrespondingServer(int socket)
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    size_t serverNumber = 0;
    
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
                break;
            }
        }
        serverNumber++;
    }
    if (serverNumber == _listServers.size())
        throw(std::exception());
    return (_listServers[serverNumber]);
}

void Http::reply(int socket, HttpRequest *received, HttpResponse *response, Server* server)
{
    std::string type;
    std::string path;
    std::string httpVersion;
    std::string content;

    //analyze request
    std::istringstream request(received->getRequestType());
    request >> type >> path >> httpVersion;
    //check version
    if (httpVersion != "HTTP/1.1")
        throw(std::exception());

    //define location
    std::map<std::string, Location *> possibleLocations = server->getLocations();
    std::map<std::string, Location *>::iterator it = this->findLocation(possibleLocations, path);
    if (it == possibleLocations.end())
    {
        DIR * root;
        root = opendir(((it->second)->getRoot()).c_str());
        if (root == NULL)
            throw(std::exception());
        
        //check method
        size_t i = 0;
        while ( i < (server->getAllowedMethods()).size())
        {
            if ((server->getAllowedMethods())[i] == type)
            {
                if (type == "GET")
                {
                    response->writeContent(path, server);
                    response->setGetHeader();
                }
                else if (type == "POST")
                {
                    response->setLength(0);
                    response->setContent("");
                    response->setPostHeader();
                }
                else
                {
                    response->setLength(0);
                    response->setContent("");
                    response->setDeleteHeader();
                }
                break;
            }
        }
        if (i == (server->getAllowedMethods()).size())
            throw(std::exception());
    }
    send(socket, response->getHeader().c_str(), response->getHeader().size(), 0);
    send(socket, response->getContent().c_str(), response->getContent().size(), 0);

    //define connection type

    //searchfile

    //prepare reply
    
}

std::map<std::string, Location *>::iterator Http::findLocation(std::map<std::string, Location *> &possibleLocations, std::string path)
{
    std::map<std::string, Location *>::iterator it = possibleLocations.begin();
    // size_t locationNum = 0;
    (void) path;

    while (it != possibleLocations.end()) 
    {
        // if (*(path.cend() - 1) == '/')
        // {
        //     DIR * root;
        //     root = opendir(((it->second)->getRoot() + path).c_str());
        //     if (root != NULL)
        //     {
                
        //     }
        //     if (it->second->getAutoIndex())
        //     {
        //         dirent *picas = readdir(root);
        //         picas->d_name;
        //     }
            
        // }    
            //it is a path to directory
        it++;
    }
    return it;
}