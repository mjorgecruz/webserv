/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiManagement.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 19:10:43 by masoares          #+#    #+#             */
/*   Updated: 2024/12/12 17:59:32 by masoares         ###   ########.fr       */
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
        std::string length = "CONTENT_LENGTH=";
        length += (request.getRequestBody().size());
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
    (void) request;
    std::vector<char*> envp;

    envp.push_back(const_cast <char *> ( requ.c_str()));
    std::string protocol = "SERVER_PROTOCOL=HTTP/1.1";
    envp.push_back(const_cast <char *> ( protocol.c_str()));
    std::string pathInfo = "PATH_INFO=" + file;
    envp.push_back(const_cast <char *> ( pathInfo.c_str()));
    envp.push_back(NULL);

    char outputTemplate[] = "/tmp/cgi_output_XXXXXX";
    int outputFd = mkstemp(outputTemplate);
    if (outputFd == -1)
        throw std::exception();

    int pid = fork();
    if (pid == -1)
    {
        close(outputFd);
        throw std::exception();
    }
    if (pid == 0)
    {

        int inputFd = open(file.c_str(), O_RDONLY);
        dup2(inputFd, STDIN_FILENO);
        dup2 (outputFd, STDOUT_FILENO);
    
        close(inputFd);
        close(outputFd);
        
        std::vector<char*> args;
        args.push_back(strdup(info._cgiPath.c_str()));
        args.push_back(strdup(file.c_str()));
        args.push_back(NULL);
        alarm(10);
        try{
            if (execve(args[0], args.data(), envp.data()) == -1)
            {
                std::cerr << "execve error" << std::endl;
                free(args[0]);
                free(args[1]);
                exit(EXIT_FAILURE);
            }
        }
        catch (...)
        {
            free(args[0]);
            free(args[1]);
            exit(1000);
            alarm(0);
        }
    }
    int status;
    waitpid(pid, &status, 0);
    if (status == 14 || status == EXIT_FAILURE)
    {
        content = "Status: 500\r\nContent-Type: text/html\r\n\r\n<html><body><h1>500 Internal Server Error</h1><p>Server misconfiguration.</p></body></html>";
        return;
    }

    char buffer[1024];
    ssize_t bytes_read;
    lseek(outputFd, 0, SEEK_SET);
    while ((bytes_read = read(outputFd, buffer, sizeof(buffer) - 1)) != 0)
    {
        if (bytes_read == -1)
            break;
        buffer[bytes_read] = '\0';
        content += buffer;
    }
    close(outputFd);
    unlink(outputTemplate);
}

void CgiManagement::postCgiTester(std::string requ, std::string file, t_info &info, std::string &content, HttpRequest &request)
{   
    (void) request;  
    std::vector<char*> envp;

    envp.push_back(const_cast <char *> ( requ.c_str()));
    std::string protocol = "SERVER_PROTOCOL=HTTP/1.1";
    envp.push_back(const_cast <char *> ( protocol.c_str()));
    std::string pathInfo = "PATH_INFO=" + file;
    envp.push_back(const_cast <char *> ( pathInfo.c_str()));
    std::string special = "";
    if (request.searchProperty("X-Secret-Header-For-Test") != "undefined")
    {
        special = "HTTP_X_SECRET_HEADER_FOR_TEST=" + request.searchProperty("X-Secret-Header-For-Test");
        std::cout << special << std::endl;
        envp.push_back(const_cast <char *> ( special.c_str()));
    }
    envp.push_back(NULL);
    
    char inputTemplate[] = "/tmp/cgi_input_XXXXXX";
    char outputTemplate[] = "/tmp/cgi_output_XXXXXX";
    int inputFd = mkstemp(inputTemplate);
    int outputFd = mkstemp(outputTemplate);
    if (inputFd == -1 || outputFd == -1)
    {
        throw std::exception();
    }
    std::string body = request.getRequestBody();
    if(write(inputFd, body.c_str(), body.size()) == -1)
    {
        close(inputFd);
        close(outputFd);
        throw std::exception();
    }
    lseek(inputFd, 0, SEEK_SET);
    
    int pid = fork();
    if (pid == -1)
    {
        close(inputFd);
        close(outputFd);
        throw std::exception();
    }
    if (pid == 0)
    {
        dup2(inputFd, STDIN_FILENO);
        dup2(outputFd, STDOUT_FILENO);
        close(inputFd);
        close(outputFd);
        
        std::vector<char*> args;
        args.push_back(strdup(info._cgiPath.c_str()));
        args.push_back(strdup(file.c_str()));
        args.push_back(NULL);
        
        signal(SIGALRM, handle_alarm);
        alarm(10);
        try{
            if (execve(args[0], args.data(), envp.data()) == -1)
            {
                std::cerr << "execve error" << std::endl;
                free(args[0]);
                free(args[1]);
                exit(EXIT_FAILURE);
            }
        }
        catch (...)
        {
            exit(1000);
            alarm(0);
        }
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        if (status == 14 || status == EXIT_FAILURE)
        {
            content = "Status: 500\r\nContent-Type: text/html\r\n\r\n<html><body><h1>500 Internal Server Error</h1><p>Server misconfiguration.</p></body></html>";
            return;
        }
        close(inputFd);
        
        char buffer[1024];
        ssize_t bytes_read;

        lseek(outputFd, 0, SEEK_SET);
        //std::ofstream outputFile(file.c_str());
        while ((bytes_read = read(outputFd, buffer, sizeof(buffer) - 1)) != 0)
        {
            // if (bytes_read == -1)
            //     break;
            buffer[bytes_read] = '\0';
            content += buffer;
           // outputFile << buffer;
        }
        close(outputFd);
        //outputFile.close();
        unlink(inputTemplate);
        unlink(outputTemplate);
        info._status = 200;
    }
}