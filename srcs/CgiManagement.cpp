/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiManagement.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 19:10:43 by masoares          #+#    #+#             */
/*   Updated: 2024/11/27 12:00:33 by masoares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiManagement.hpp"

CgiManagement::CgiManagement()
{}
         
CgiManagement::~CgiManagement()
{}

void CgiManagement::solveCgiPhp(std::string file, t_info &info, std::string &content)
{
    (void) info;
    (void) content;
    int fd[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) == -1)
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
        // std::vector<char*> envp;
        // std::string envVar = "REQUEST_METHOD=GET";
        // std::string envVar1 = "SERVER_PROTOCOL=HTTP/1.1";
        // std::string envVar2 = "PATH_INFO=";
        // envVar2 += file;
        
        
        // envp.push_back(const_cast <char *> ( envVar.c_str()));
        // envp.push_back(const_cast <char *>(envVar1.c_str()));
        // envp.push_back(const_cast <char *>(envVar2.c_str()));
        // envp.push_back(NULL);
            std::map<std::string, std::string> env;
        env["CONTENT_LENGTH"] = std::to_string(request->getBody().size());
        env["CONTENT_TYPE"] = request->getHeader("Content-Type");
        env["GATEWAY_INTERFACE"] = "CGI/1.1";
        env["PATH_INFO"] = request->getPath();
        //env["QUERY_STRING"] = request->getQueryString();
        //env["REMOTE_ADDR"] = request->getRemoteAddr();
        env["REQUEST_METHOD"] = request->getRequestType();
        env["SCRIPT_NAME"] = cgiScriptPath;
        env["SERVER_NAME"] = request->getServerName();
        env["SERVER_PORT"] = std::to_string(request->getServerPort());
        env["SERVER_PROTOCOL"] = "HTTP/1.1";
        env["SERVER_SOFTWARE"] = "webserv/1.0";
    
        // Convert environment variables to char* array
        std::vector<char*> envp;
        for (c : env) {
            std::string envVar = e.first;
            envVar += "=";
            envVar += e.second;
            envp.push_back(const_cast <char *> ( envVar.c_str()));
        }
        envp.push_back(NULL);
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        const char * args[3] = {info._cgiPath.c_str(),  file.c_str(), NULL};
        if(execve(args[0], const_cast<char* const*>(args), envp.data()) == -1)
        {
            exit(EXIT_FAILURE);
        }
    }
    int status;
    close(fd[1]);
    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(fd[0], buffer, sizeof(buffer) - 1)) != 0)
    {
        if (bytes_read == -1)
            throw std::exception();
        buffer[bytes_read] = '\0';
        content += buffer;
    }
    close(fd[0]);
    waitpid(pid, &status, 0);
}