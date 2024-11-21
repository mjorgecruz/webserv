/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 13:37:26 by masoares          #+#    #+#             */
/*   Updated: 2024/11/21 10:43:15 by masoares         ###   ########.fr       */
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

        //exit(1);
        ///////////////////////////////////////////////////
        //comment this exit to make the program run again//
        //uncomment it to run the make test command      //
        ///////////////////////////////////////////////////
                
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
            else if ( events[i].events & (EPOLLIN | EPOLLET ))
            {
                bool isServerSocket = false;
                for (size_t j = 0; j < _listServers.size(); j++)
                {
                    if (events[i].data.fd == _listServers[j]->getSocketFd())
                    {
                        accept_new_connection(_listServers[j]->getSocketFd(), _epollFd);
                        isServerSocket = true;
                        break;
                    }
                }
                if (!isServerSocket)
                {
                    data_transfer(events[i].data.fd);
                }
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
    int flags = fcntl(client_fd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Error getting socket flags: " << strerror(errno) << std::endl;
        return;
    }
    if (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "Error setting socket to non-blocking: " << strerror(errno) << std::endl;
    }
    event.data.fd = client_fd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1)
    {
        std::cerr << "Failed to add client socket to epoll: " << strerror(errno) << std::endl;
        close(client_fd);
    }
    std::cout<< "Added: " << client_fd << std::endl;
}

void Http::data_transfer(int socket)
{
    HttpRequest *request = new HttpRequest();

    int server_fd = 0;

    //check for the corresponding server
    std::vector<Server *> correspondingServers;
    correspondingServers = findCorrespondingServer(socket); 
    bool completed = request->completeRequest(socket);
    if (completed)
    {
        request->fillReqProperties();
        request->defineMimeType();
        Server *correctServer = findCorrectServerName(request, correspondingServers);
        server_fd = correctServer->getSocketFd();

        //fill request properties
        request->setClientFd(server_fd);
        
        //prepare response
        HttpResponse *response = new HttpResponse(socket, correctServer);
        try{
            this->reply(socket, request, response, correctServer);
        }
        catch (std::exception &e)
        {
            std::cerr << "Bad Request" << std::endl;
            response->setStatus(403);
            response->setLength(0);
            response->setPostHeader();
            sendData(socket, response);
        }
        delete(response);
        delete(request);    
    }
    
}

std::vector<Server *> Http::findCorrespondingServer(int socket)
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    size_t serverNumber = 0;
    std::vector<Server *> correspondingServers;
    
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
                correspondingServers.push_back(_listServers[serverNumber]);
            }
        }
        serverNumber++;
    }
    if (serverNumber == _listServers.size() && correspondingServers.empty())
        throw(std::exception());
    return (correspondingServers);
}

Server * Http::findCorrectServerName(HttpRequest *request, std::vector<Server *> &correspondingServers)
{
    for (size_t i = 0; i < correspondingServers.size(); i++)
    {
        for (size_t j = 0; j < (correspondingServers[i]->getHostname()).size(); j++)
        {
            if ((correspondingServers[i]->getHostname())[j] == request->searchProperty("Host"))
                return correspondingServers[i];
        }
    }
    return correspondingServers[0];
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
    std::vector<std::pair <std::string, Location *> > possibleLocations = server->getLocations();
    std::vector<std::pair <std::string, Location *> >::iterator it = this->findLocation(possibleLocations, path);

    //define all properties considering path received
    
    t_info Info;
    Info._status = 0;
    if(it != possibleLocations.end())
        fillStructInfo(Info, server, it->second);
    else
        fillStructInfo(Info, server, NULL);

    // to print all the fields to console for debuging
    Info.printInfoConfig();
    
    std::string full_path = Info._root + path;
    struct stat entryInfo;
    if (stat(full_path.c_str(), &entryInfo) == 0)
    {
        if (S_ISDIR(entryInfo.st_mode))
            path = path + "/";       
    }
    //check method
    if (!(Info._redirect.empty()))
    {
        response->writeRedirectContent(Info, received);
        response->setGetRedirectHeader(Info);
        sendData(socket, response);
        return;
    }
    size_t i = 0;
        while ( i < Info._allowedMethods.size())
        {
            if (Info._allowedMethods[i] == type)
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
                    if (Info._redirect.empty())
                        handlePost.handleDataUpload(path, *received, Info);
                    // if(!Info._cgiPath.empty())
                    // {
                    //     postCgiPage(path, info);
                    // }
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
        if (i == Info._allowedMethods.size())
            throw(std::exception());

    sendData(socket, response);
    
}

std::vector<std::pair <std::string, Location *> >::iterator Http::findLocation(std::vector<std::pair <std::string, Location *> > &possibleLocations, std::string path)
{
    std::vector<std::pair <std::string, Location *> >::iterator it = possibleLocations.begin();
    while (it != possibleLocations.end())
    {
        //name contains **
        if (it->second->getPath().find('*') != std::string::npos)
        {
            //all input
            if (it->second->getPath().size() == 1)
                return it;
            //prefix + * + suffix
            std::string pre = it->second->getPath().substr(0, it->second->getPath().find('*') - 1);
            std::string suf = it->second->getPath().substr(it->second->getPath().find('*') - 1, it->second->getPath().size() - 1 - it->second->getPath().find('*') - 1) ; 
            if (path.rfind(pre, 0) == 0 && (suf.empty() || path.find(suf, pre.size()) != std::string::npos))
            {
                return it;
            }
            else if (path.rfind(suf) < path.size() - suf.size())
                return it;
        }
        //if it matches up to the end or up to a "/"
        else
        {
            if (path.rfind(it->second->getPath(), 0) == 0)
                return it;
        }
        it++;
    }
    return possibleLocations.end();
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
        if (!location->getIndex().empty())
            Info._index = location->getIndex();
        else   
            Info._index = server->getIndex();
        
        //Error pages
        Info._errorPages = server->getErrorPages();
        std::map<int, std::string> pages = location->getErrorPages();
        std::map<int, std::string>::iterator it = pages.begin();
        while( it != pages.end())
        {
            Info._errorPages[it->first] = it->second;
            ++it;
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
    if (response->getLength() != 0)
    {
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
    
    
}