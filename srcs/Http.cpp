/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 13:37:26 by masoares          #+#    #+#             */
/*   Updated: 2024/11/09 12:16:40 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
    std::ifstream file(confPath.c_str());
    if(file)
    {
        std::string line;
        
        while (std::getline(file, line))
        {
            if (line.find_first_not_of(" \t") == std::string::npos)
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
                    throw(std::exception());
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
            _listServers[i]->printConfig();
        }
        
        for (size_t i = 0; i < _listServers.size(); i++)
        {
            _listServers[i]->createSocket(_listServers[i]->getPorts(), _listServers[i]->getHost());
        }
        this->addServersToEpoll();
        
        return ;
    }
    Server *server = new Server();
    server->setConfigs("");
    this->addServerToList(server);
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
            if (events[i].events  & (EPOLLRDHUP | EPOLLERR | EPOLLHUP))
            {
                std::cout << "Page was hard refreshed" << std::endl;
                close(events[i].data.fd);
            }
            else if (events[i].data.fd == this->_listServers[this->_listServers.size() - 1]->getSocketFd())
                accept_new_connection(this->_listServers[this->_listServers.size() - 1]->getSocketFd(), _epollFd);
            else if ( events[i].events & (EPOLLIN | EPOLLET ))
            {
                data_transfer(events[i].data.fd);
            }
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

    //fill request properties
    request->completeRequest(socket);
    request->setClientFd(server_fd);
    
    //prepare response
    HttpResponse *response = new HttpResponse(socket, correspondingServer);

    this->reply(socket, request, response, correspondingServer);

    delete(response);
    delete(request);
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
    std::cout << received->getRequestType() << std::endl;
    std::istringstream request(received->getRequestType());
    request >> type >> path >> httpVersion;
    
    //check version
    if (httpVersion != "HTTP/1.1")
        throw(std::exception());

    //define location
    std::map<std::string, Location *> possibleLocations = server->getLocations();
    std::map<std::string, Location *>::iterator it = this->findLocation(possibleLocations, path);

    //define all properties considering path received
    
    t_info Info;
    Info._status = 0;
    if(it != possibleLocations.end())
        fillStructInfo(Info, server, it->second);
    else
        fillStructInfo(Info, server, NULL);

    DIR * root;
    root = opendir(server->getRoot().c_str());
    if (root == NULL)
        throw(std::exception());
    closedir(root);
        
    //check method
    size_t i = 0;
    while ( i < (server->getAllowedMethods()).size())
    {
        if ((server->getAllowedMethods())[i] == type)
        {
            if (type == "GET")
            {
                response->setContentType(received->getMimeType());
                response->writeContent(path, Info);
                response->setGetHeader();
            }
            else if (type == "POST")
            {
                InputHandler handlePost;
                handlePost.handleDataUpload(path, *received, Info);
                response->setStatus(Info._status);
                response->setLength(0);
                response->setPostHeader();
            }
            else
            {
                DeleteHandler handleDelete;
                handleDelete.handleDataDeletion(path, *received, Info);
                response->setStatus(Info._status);
                response->setLength(0);
                response->setDeleteHeader();
            }
            break;
        }
        i++;
    }
    if (i == (server->getAllowedMethods()).size())
        throw(std::exception());
    sendData(socket, response);
    
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

void Http::fillStructInfo(t_info &Info, Server *server, Location *location)
{
    //Hostname, host and port
    Info._host = server->getHost();
    Info._ports = server->getPorts();
    Info._hostname = server->getHostname();
    Info._maxBodySize = server->getMaxBodySize();
    
    if (location != NULL)
    {
        //Root
        if (!location->getRoot().empty())
            Info._root = location->getRoot();
        else
            Info._root = server->getRoot();

        //Index
        if (location->getIndex().empty())
            Info._index = location->getIndex();
        else   
            Info._index = server->getIndex();
        
        //Error pages
        Info._errorPages = server->getErrorPages();
        for( std::map<int, std::string>::iterator it = location->getErrorPages().begin(); it != location->getErrorPages().end(); ++it)
        {
            Info._errorPages[it->first] = it->second;
        }

        //Allowed Methods
        if (!location->getAllowedMethods().empty())
            Info._allowedMethods = location->getAllowedMethods();
        else
            Info._allowedMethods = server->getAllowedMethods();

        //AutoIndex
        if (location->getAutoIndex() < 0)
        {
            if (server->getAutoIndex() <= 0)
                Info._autoIndex = 0;
            else
                Info._autoIndex = 1;
        }
        else
            Info._autoIndex = location->getAutoIndex();
        
        Info._cgiPath = location->getCgiPath();
        Info._redirect = location->getRedirect();
        Info._path = location->getPath();
    }
    else
    {
        Info._root = server->getRoot();
        Info._index = server->getIndex();
        Info._errorPages = server->getErrorPages();
        Info._allowedMethods = server->getAllowedMethods();
        
        if (server->getAutoIndex() <= 0)
            Info._autoIndex = 0;
        else
            Info._autoIndex = 1;
    }
}

void Http::sendData(int socket, HttpResponse *response)
{
    int maxRetries = 5;
    for( int i = 0; i < maxRetries; i++ )
    {
        ssize_t result = send(socket, response->getHeader().c_str(), response->getHeader().size(), MSG_NOSIGNAL);
        if (result != -1)
            break;
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
            usleep(1000);
        else
        {
            std::cerr << "Send error: " << strerror(errno) << std::endl;
            return;
        }
        std::cerr << "Failed to send after " << maxRetries << " retries." << std::endl;
    }
    for( int i = 0; i < maxRetries; i++ )
    {
        ssize_t result = send(socket, response->getContent().c_str(), response->getContent().size(), MSG_NOSIGNAL);;
        if (result != -1)
            return;
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
            usleep(1000);
        else
        {
            std::cerr << "Send error: " << strerror(errno) << std::endl;
            close(socket);
            return;
        }
        std::cerr << "Failed to send after " << maxRetries << " retries." << std::endl;
    }
    
    
}