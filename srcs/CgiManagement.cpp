/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiManagement.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 19:10:43 by masoares          #+#    #+#             */
/*   Updated: 2024/12/02 12:19:41 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiManagement.hpp"

CgiManagement::CgiManagement()
{}
         
CgiManagement::~CgiManagement()
{}

void CgiManagement::solveCgiPhp(std::string file, t_info &info, std::string &content, HttpRequest &request)
{
    (void) info;
    (void) content;
    (void) request;
    int fdin[2];
    int fdout[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fdin) == -1)
    {
        std::exception();
    }
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fdout) == -1)
    {
        std::exception();
    }
    int pid = fork();
    if (pid == -1)
    {
        std::exception();
    }
    if (pid == 0)
    {
        close(fdin[1]);
        dup2(fdin[0], STDIN_FILENO);
        close(fdin[0]);

        close(fdout[0]);
        dup2(fdout[1], STDOUT_FILENO);
        close(fdout[1]);
        
        std::vector<char*> envp;
        std::istringstream req(request.getRequestType());
        std::string type;
        std::string requ = "REQUEST_METHOD=";
        req >> type;
        requ+=type;
        envp.push_back(const_cast <char *> ( requ.c_str()));
        std::string protocol = "SERVER_PROTOCOL=HTTP/1.1";
        std::string contentType = "CONTENT_TYPE=" + request.searchProperty("Content-Type");
        envp.push_back(const_cast<char*>(contentType.c_str()));
        std::string gatewayInterface = "GATEWAY_INTERFACE=CGI/1.1";
        envp.push_back(const_cast<char*>(gatewayInterface.c_str()));
        envp.push_back(const_cast <char *> ( protocol.c_str()));
        std::string length = "CONTENT_LENGTH=" + (request.getRequestBody().size());
        envp.push_back(const_cast <char *> ( length.c_str()));
        req >> type;
        std::string pathInfo = "PATH_INFO=" + file;
        envp.push_back(const_cast <char *> ( pathInfo.c_str()));
        std::string pathTran = "PATH_TRANSLATED=" + info._cgiPath;
        envp.push_back(const_cast <char *> ( pathTran.c_str()));
        std::string queryString = "QUERY_STRING=";
        envp.push_back(const_cast<char*>(queryString.c_str()));
        std::string script_name = "SCRIPT_NAME=" + info._cgiPath;
        envp.push_back(const_cast <char *> ( script_name.c_str()));
        std::string request_uri = "REQUEST_URI=" + file;
        envp.push_back(const_cast <char *> ( request_uri.c_str()));
        std::string script_file = "SCRIPT_FILENAME=" + file;
        envp.push_back(const_cast <char *> ( script_file.c_str()));
        envp.push_back(NULL);
        
        for(int i = 0; envp[i] != NULL; i++)
            std::cout << envp[i] << std::endl;
        

        char **args = (char **) malloc( sizeof(char *) * 3);
        args[0] = strdup(info._cgiPath.c_str());
        args[1] = strdup(file.c_str());
        args[2] = NULL;
        if(execve(args[0], args, envp.data()) == -1)
        {
            std::cout << "execve error" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    close(fdin[0]);
    close(fdout[1]);
    write(fdin[1], request.getRequestBody().c_str(),request.getRequestBody().size());
    int status;
    close(fdin[1]);
    fcntl(fdout[0], F_SETFL, O_NONBLOCK);
    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(fdout[0], buffer, sizeof(buffer) - 1)) != 0)
    {
        if (bytes_read == -1)
            break;
            //throw std::exception();
        buffer[bytes_read] = '\0';
        content += buffer;
    }
    close(fdout[0]);
    waitpid(pid, &status, 0);
}

void CgiManagement::solveCgiTester(std::string file, t_info &info, std::string &content, HttpRequest &request)
{
    std::istringstream req(request.getRequestType());
    std::string type;
    std::string requ = "REQUEST_METHOD=";
    req >> type;
    requ+=type;
    
    if (type == "GET")
    {
        getCgiTester(requ, file, info, content, request);
    }
    else if (type == "POST")
    {
        postCgiTester(requ, file, info, content, request);
    }
}

void CgiManagement::getCgiTester(std::string requ, std::string file, t_info &info, std::string &content, HttpRequest &request)
{
    int fdout[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fdout) == -1)
    {
        std::exception();
    }
    int pid = fork();
    if (pid == -1)
    {
        std::exception();
    }
    if (pid == 0)
    {
        int fileFd = open(file.c_str(), O_RDONLY);
        if (fileFd > 0)
        {
            dup2(fileFd, STDIN_FILENO);
        }
        close(fileFd);
        close(fdout[0]);
        dup2(fdout[1], STDOUT_FILENO);
        close(fdout[1]);
        
        std::vector<char*> envp;

        envp.push_back(const_cast <char *> ( requ.c_str()));
        std::string protocol = "SERVER_PROTOCOL=HTTP/1.1";
        std::string contentType = "CONTENT_TYPE=" + request.searchProperty("Content-Type");
        envp.push_back(const_cast<char*>(contentType.c_str()));
        std::string gatewayInterface = "GATEWAY_INTERFACE=CGI/1.1";
        envp.push_back(const_cast<char*>(gatewayInterface.c_str()));
        envp.push_back(const_cast <char *> ( protocol.c_str()));
        std::string length = "CONTENT_LENGTH=" + (request.getRequestBody().size());
        envp.push_back(const_cast <char *> ( length.c_str()));
        std::string pathInfo = "PATH_INFO=" + file;
        envp.push_back(const_cast <char *> ( pathInfo.c_str()));
        std::string pathTran = "PATH_TRANSLATED=" + info._cgiPath;
        envp.push_back(const_cast <char *> ( pathTran.c_str()));
        std::string queryString = "QUERY_STRING=";
        envp.push_back(const_cast<char*>(queryString.c_str()));
        std::string script_name = "SCRIPT_NAME=" + info._cgiPath;
        envp.push_back(const_cast <char *> ( script_name.c_str()));
        std::string request_uri = "REQUEST_URI=" + file;
        envp.push_back(const_cast <char *> ( request_uri.c_str()));
        std::string script_file = "SCRIPT_FILENAME=" + file;
        envp.push_back(const_cast <char *> ( script_file.c_str()));
        envp.push_back(NULL);
        
        for(int i = 0; envp[i] != NULL; i++)
            std::cout << envp[i] << std::endl;
        

        char **args = (char **) malloc( sizeof(char *) * 3);
        args[0] = strdup(info._cgiPath.c_str());
        args[0] = strdup(file.c_str());
        args[1] = NULL;
        if(execve(args[0], args, envp.data()) == -1)
        {
            std::cout << "execve error" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    close(fdout[1]);
    int status;
    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(fdout[0], buffer, sizeof(buffer) - 1)) != 0)
    {
        if (bytes_read == -1)
            break;
            //throw std::exception();
        buffer[bytes_read] = '\0';
        content += buffer;
    }
    close(fdout[0]);
    std::cout << content << std::endl;
    waitpid(pid, &status, 0);
}

void CgiManagement::postCgiTester(std::string requ, std::string file, t_info &info, std::string &content, HttpRequest &request)
{
    (void) info;
    (void) content;
    (void) request;
        
    std::vector<char*> envp;

    envp.push_back(const_cast <char *> ( requ.c_str()));
    std::string protocol = "SERVER_PROTOCOL=HTTP/1.1";
    envp.push_back(const_cast <char *> ( protocol.c_str()));
    std::string pathInfo = "PATH_INFO=" + file;
    envp.push_back(const_cast <char *> ( pathInfo.c_str()));
    envp.push_back(NULL);
    
    int fdin[2];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fdin) == -1)
    {
        std::exception();
    }
    int pid = fork();
    if (pid == -1)
    {
        std::exception();
    }
    if (pid == 0)
    {
        close(fdin[1]);
        dup2(fdin[0], STDIN_FILENO);
        int fileFd = open(file.c_str(), O_RDWR);
        if (fileFd < 0)
            throw(std::exception());
        dup2(fileFd, STDOUT_FILENO);
        close(fdin[0]); 
        
        for(int i = 0; envp[i] != NULL; i++)
            std::cout << envp[i] << std::endl;
        
        char **args = (char **) malloc( sizeof(char *) * 3);
        args[0] = strdup(info._cgiPath.c_str());
        args[1] = strdup(file.c_str());
        args[2] = NULL;
        if(execve(args[0], args, envp.data()) == -1)
        {
            std::cout << "execve error" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        int status;
        close(fdin[0]);
        int total_written = 0;
        int body_size = request.getRequestBody().size();

        int chunk_size = 10000; // 64 KB
        const char *body_data = request.getRequestBody().c_str();
        while (total_written < body_size)
        {
            int to_write = std::min(chunk_size, body_size - total_written);
            int written = write(fdin[1], body_data + total_written, to_write);
            total_written += written;
            std::cout << total_written << std::endl;
        }
        close(fdin[1]);
        waitpid(pid, &status, 0);
        
        char buffer[1024];
        ssize_t bytes_read;
        

        int fileFd = open(file.c_str(), O_RDWR);
        if (fileFd < 0)
            throw(std::exception());
        while ((bytes_read = read(fileFd, buffer, sizeof(buffer) - 1)) != 0)
        {
            if (bytes_read == -1)
                break;
                //throw std::exception();
            buffer[bytes_read] = '\0';
            content += buffer;
            if (content.find("\r\n\r\n") != std::string::npos)
                content.substr(0, content.find("\r\n\r\n"));
        }
        close(fileFd);
        
    }
}