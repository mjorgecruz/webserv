/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 13:37:26 by masoares          #+#    #+#             */
/*   Updated: 2024/12/12 11:35:40 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http.hpp"

Http::Http( void )
{
	int epoll_fd = epoll_create(1);

	if (epoll_fd == -1) {
        std::cout << "\033[33mHttp() ->  HTTP.cpp @ex \033[0m" << std::endl;
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
                catch (Server::exceptionAtServer &e)
                {
                    delete server;
                    std::cout << "\033[1;31mAt serverChecker ->  HTTP.cpp @ex \033[0m" << std::endl;
                    throw(std::exception());
                }
                
                if (server)
                    addServerToList(server);
                else
                {
                    std::cout << "\033[1;31maddServerToList ->  HTTP.cpp @ex \033[0m" << std::endl;
                    throw(std::exception());
                }
            }
        }
        file.close();
    
        for (size_t i = 0; i < _listServers.size(); i++)
        {
            _listServers[i]->printConfig();
        }

        // exit(1);
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
    event.events = EPOLLIN ;
	event.data.fd = server->getSocketFd();
    
    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, server->getSocketFd(), &event) == -1)
    {
        std::cout << "\033[33mepoll_ctl ->  HTTP.cpp @ex \033[0m" << std::endl;
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
    {
        std::cout << "\033[33moperator[] ->  HTTP.cpp @ex \033[0m" << std::endl;
        throw(std::exception());
    }
    return this->_listServers[num];
}

int Http::listServersSize() const
{
    return this->_listServers.size();
}

void Http::runApplication()
{
    struct epoll_event events[1024];
    int event_count;
    std::map<int, HttpRequest*> requests;
    
    while (g_signal == 0)
    {
        event_count = epoll_wait(_epollFd, events, 1024, 1000);
        
        for (int i = 0; i < event_count; i++) 
        {
            int fd = events[i].data.fd;

            if (events[i].events  & (EPOLLRDHUP | EPOLLERR | EPOLLHUP))
            {
                std::cout << "Page was hard refreshed" << std::endl;
                if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, events[i].data.fd, NULL) == -1)
                {
                    std::cerr << "Failed to remove file descriptor from epoll instance: " << strerror(errno) << std::endl;
                }
                close(fd);
                if (requests.find(fd) != requests.end())
                {
                    delete requests[fd];
                    requests.erase(fd);
                }
            }
            else if ( events[i].events & (EPOLLIN))
            {
                bool isServerSocket = false;
                for (size_t j = 0; j < _listServers.size(); j++)
                {
                    if (fd == _listServers[j]->getSocketFd())
                    {
                        accept_new_connection(_listServers[j]->getSocketFd(), _epollFd);
                        isServerSocket = true;
                        break;
                    }
                }
                if (!isServerSocket)
                {
                    if (requests.find(fd) == requests.end())
                    {
                        requests[fd] = new HttpRequest();
                    }
                    //std::cout << "TRANSFERRING-------------------------------------------------" << std::endl;

                    data_transfer(fd, events[i], requests[fd]);
                    if (requests[fd]->completed)
                    {
                        delete requests[fd];
                        requests.erase(fd);
                    }
                }
            }
		}
    }
    for (std::map<int, HttpRequest*>::iterator it = requests.begin(); it != requests.end(); ++it)
    {
        delete it->second;
    }
    requests.clear();
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
    // int flags = fcntl(client_fd, F_GETFL, 0);
    // if (flags == -1) {
    //     std::cerr << "Error getting socket flags: " << strerror(errno) << std::endl;
    //     return;
    // }
    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
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

void Http::data_transfer(int socket, struct epoll_event &event, HttpRequest * request)
{
    int server_fd = 0;
    (void) event;

    //check for the corresponding server
    std::vector<Server *> correspondingServers;
    correspondingServers = findCorrespondingServer(socket); 
    try{
        request->completed = request->completeRequest(socket);
    }
    catch (std::exception &e)
    {
        std::cerr << "Connection timed out" << std::endl;
        Server *correctServer = findCorrectServerName(request, correspondingServers);
        server_fd = correctServer->getSocketFd();
        HttpResponse *response = new HttpResponse(socket, correctServer);
        response->setStatus(408);
        response->writeError408();
        response->setGetHeader("");
        sendData(socket, response);
        delete(response);
        return;
    }

    if (request->completed)
    {
        request->fillReqProperties();
        request->defineMimeType();
        Server *correctServer = findCorrectServerName(request, correspondingServers);
        server_fd = correctServer->getSocketFd();

        //fill request properties
        request->setClientFd(server_fd);
        
        //prepare response
        HttpResponse *response = new HttpResponse(socket, correctServer);
        
        //session config
        std::string sessionId = allSessions.sessionConfig(*request);
        
        try{
            this->reply(socket, request, response, correctServer, sessionId);
        }
        catch (std::exception &e)
        {
            std::cerr << "Bad Request" << std::endl;
            response->setStatus(405);
            response->setLength(0);
            response->setPostHeader(sessionId);
            sendData(socket, response);
        }
        delete(response); 
    }
    
}

std::vector<Server *> Http::findCorrespondingServer(int socket)
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    size_t serverNumber = 0;
    std::vector<Server *> correspondingServers;
    
    if (getsockname(socket, (sockaddr *) &addr, &len) == -1)
    {
        std::cout << "\033[33mgetsockname ->  HTTP.cpp @ex \033[0m" << std::endl;
        throw(std::exception());
    }
    int port = ntohs(addr.sin_port);
    std::string address = inet_ntoa(addr.sin_addr);
    
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
    {
        std::cout << "\033[33mfindCorrespondingServer() ->  HTTP.cpp @ex \033[0m" << std::endl;
        throw(std::exception());
    }
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


void Http::reply(int socket, HttpRequest *received, HttpResponse *response, Server* server, std::string sessionId)
{
    std::string type;
    std::string path;
    std::string httpVersion;
    std::string content;

    //analyze request
    std::istringstream request(received->getRequestType());
    request >> type >> path >> httpVersion;
    

    //define location
    std::vector<std::pair <std::string, Location *> > possibleLocations = server->getLocations();
    std::vector<std::pair <std::string, Location *> >::iterator it = this->findLocation(possibleLocations, path);

    //define all properties considering path received
    t_info Info;
    Info._status = 0;
    Info._maxBodySize = 0;
    if(it != possibleLocations.end())
        fillStructInfo(Info, server, it->second);
    else
        fillStructInfo(Info, server, NULL);

    //check version
    if (httpVersion != "HTTP/1.1")
    {
        std::cout << "\033[33mreply() ->  HTTP.cpp @ex \033[0m" << std::endl;
        std::cerr << "Unsupported Http version" << std::endl;
        response->setStatus(505);
        response->writeErrorPage(Info, 505);
        response->setGetHeader(sessionId);
        sendData(socket, response);
        return;
    }
    
    // to print all the fields to console for debuging
    //Info.printInfoConfig();
    
    std::string  full_path;
    if (path.find("/") != 0 && Info._root.find_last_of("/") != Info._root.size() - 1)
    { 
        full_path = Info._root + "/";
        full_path += path;
    }
    else
        full_path = Info._root + path;
    struct stat entryInfo;
    if (stat(full_path.c_str(), &entryInfo) == 0)
    {
         if (S_ISDIR(entryInfo.st_mode))
            path = path + "/";
    }

    //check if auth is needed and was made
    if(!Info._authFile.empty() && !allSessions.checkSession(sessionId))
    {
        response->setStatus(401);
        response->writeErrorPage(Info, 401);
        response->setGetHeader(sessionId);
        sendData(socket, response);
        return;
    }
    else if(!(Info._root + "/.protected").empty())
    {
        allSessions.fillUsers(Info._root + "/.protected");
    }
    
    //check method
    if (!(Info._redirect.empty()))
    {
        response->writeRedirectContent(Info, received);
        response->setGetRedirectHeader(Info, sessionId);
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
                response->writeContent(path, Info, *received);
                response->setGetHeader(sessionId);
            }
            else if (type == "POST")
            {
                if ( (long) received->getRequestBody().size() > Info._maxBodySize && Info._maxBodySize > 0)
                {
                    response->setLength(0);
                    response->setStatus(413);
                    response->writeErrorPage(Info, 413);
                    response->setPostHeader(sessionId);
                }
                else
                {
                    InputHandler handlePost;
                    handlePost.handleDataUpload(path, *received, Info, *response);
                    response->setLength(response->getContent().size());
                    response->setStatus(Info._status);
                    response->setPostHeader(sessionId);

                    //session config
                    if (full_path.find("login_form") != std::string::npos)
                    {
                        this->allSessions.sessionControl(full_path, sessionId, *response, Info);
                        remove(full_path.c_str());
                    }
                    else if (full_path.find("logout_form") != std::string::npos)
                    {
                        this->allSessions.handleLogout(sessionId);
                        remove(full_path.c_str());
                    }
                    else if (full_path.find("delete_form") != std::string::npos)
                    {
                        this->allSessions.handleDelete(full_path, sessionId, *response, Info);
                        remove(full_path.c_str());
                    }
                    else if (full_path.find("create_form") != std::string::npos)
                    {
                        this->allSessions.handleCreate(full_path, sessionId, *response, Info);
                        remove(full_path.c_str());
                    }
                }
            }
            else
            {
                DeleteHandler handleDelete;
                handleDelete.handleDataDeletion(path, *received, Info);
                response->setStatus(Info._status);
                response->setLength(0);
                response->setDeleteHeader(sessionId);
            }
            break;
        }
        i++;
    }
    if (i == Info._allowedMethods.size())
    {
        std::cout << "\033[33mreply() ->  HTTP.cpp @ex \033[0m" << std::endl;
        throw(std::exception());
    }
    std::cout << response->getHeader()<< std::endl;
    sendData(socket, response);
}

std::vector<std::pair <std::string, Location *> >::iterator Http::findLocation(std::vector<std::pair <std::string, Location *> > &possibleLocations, std::string &path)
{

    std::map<std::string, std::vector<std::pair <std::string, Location *> >::iterator> locations;
    std::vector<std::pair <std::string, Location *> >::iterator it = possibleLocations.begin();
    std::string temp = path;
    if (temp.find_last_of('/') != temp.size() - 1)
        temp = path + "/";
    while (it != possibleLocations.end())
    {
        //name contains **
        std::string name;
        if (it->second->getPath().find('*') != std::string::npos)
        {
            //all input
            if (it->second->getPath().size() == 1)
                locations[path] = it;
            //prefix + * + suffix
            std::string pre = it->second->getPath().substr(0, it->second->getPath().find('*'));
            std::string suf = it->second->getPath().substr(it->second->getPath().find('*') + 1, it->second->getPath().size() - 1 - it->second->getPath().find('*')) ; 
            if (temp.rfind(pre, 0) == 0)
            {
                if (!suf.empty() && temp.find(suf, pre.size()) != std::string::npos)
                {
                    name = temp.substr(0, temp.find(suf, pre.size()) + suf.size());
                    if (name.find_last_of("/") == name.size() - 1)
                        name = name.substr(0, name.size() - 1);
                    else
                        name = name.substr(0, name.find_last_of("/"));
                }
                else if (!suf.empty())
                {
                    it++;
                    continue;
                }
                else
                {
                    name = path;
                }
                if(locations.find(name) == locations.end())
                    locations[name] = it;
            }
            else if (path.rfind(suf) < path.size() - suf.size())
            {
                name = path.substr(0, path.rfind(suf) + suf.size() );
                if(locations.find(name) == locations.end())
                    locations[name] = it;
            }
            std::cout << name << std::endl;
        }
        //if it matches up to the end or up to a "/"
        else
        {
            if (temp.rfind(it->second->getPath(), 0) == 0)
            {
                name = it->second->getPath();
                if(locations.find(name) == locations.end())
                    locations[name] = it;
            }
        }
        it++;
    }
    if (locations.size() == 0)
        return possibleLocations.end();
    else
    {
        std::map<std::string, std::vector<std::pair <std::string, Location *> >::iterator>::iterator iter = locations.begin();
        size_t max = 0;
        std::string pathToRemove;
        while (iter != locations.end())
        {
            if (iter->first.size() > max)
            {
                max = iter->first.size();
                it = iter->second;
                pathToRemove = iter->first;
            }
            iter++;
        }
        temp = temp.substr(pathToRemove.size(), temp.size());
        path = temp;
        return it;
    }
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
        //Max body size
        if (location->getMaxBodySize() != 0)
            Info._maxBodySize = location->getMaxBodySize();
        else
            Info._maxBodySize = server->getMaxBodySize();
            
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
        
        if (!location->getAuthFile().empty())
            Info._authFile = location->getAuthFile();
        else
            Info._authFile = "";
            
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
    // signal(SIGALRM, handle_alarm);
    // alarm(5);
    try 
    {
        std::string total = response->getHeader() + response->getContent(); 
        size_t dataSize = total.size();
        size_t totalSent = 0;
        const char *data;
        while (totalSent < dataSize)
        {
            data = total.c_str() + totalSent;
            ssize_t result = send(socket, data, dataSize - totalSent, 0);
            if (result > 0)
            {
                //alarm(5);
                totalSent += result;
                std::cout << "SENDING----------------------------------------------------" << std::endl;
            }
        }
    }
    catch (std::exception())
    {
        response->setStatus(504);
        response->writeError504();
        response->setGetHeader("");
        sendData(socket, response);
        close(socket);
        return;
            
    }
    //alarm(0);
}

    


const char *Http::MaxBodySizeException::what( void ) const throw()
{
    return "Max body size reached";
}