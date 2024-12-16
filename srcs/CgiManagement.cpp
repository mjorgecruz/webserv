/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiManagement.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 19:10:43 by masoares          #+#    #+#             */
/*   Updated: 2024/12/16 11:36:31 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "CgiManagement.hpp"

CgiManagement::CgiManagement()
{}
         
CgiManagement::~CgiManagement()
{}

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
        alarm(5);
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
        alarm(5);
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
        if (info._cgiPath.find("tester") != std::string::npos)
        {
            std::ofstream outputFile(file.c_str());
            while ((bytes_read = read(outputFd, buffer, sizeof(buffer) - 1)) != 0)
            {
                if (bytes_read == -1)
                    break;
                buffer[bytes_read] = '\0';
                content += buffer;
                outputFile << buffer;
            }
            close(outputFd);
            outputFile.close(); 
        }
        else
        {
            while ((bytes_read = read(outputFd, buffer, sizeof(buffer) - 1)) != 0)
            {
                if (bytes_read == -1)
                    break;
                buffer[bytes_read] = '\0';
                content += buffer;
            }
            close(outputFd);
        }
        
        unlink(inputTemplate);
        unlink(outputTemplate);
        info._status = 200;
    }
}