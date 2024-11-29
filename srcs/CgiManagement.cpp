/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiManagement.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 19:10:43 by masoares          #+#    #+#             */
/*   Updated: 2024/11/29 10:27:36 by masoares         ###   ########.fr       */
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
        std::vector<char*> envp;
        std::istringstream req(request.getRequestType());
        std::string type;
        std::string requ = "REQUEST_METHOD=";
        req >> type;
        requ+=type;
        envp.push_back(const_cast <char *> ( requ.c_str()));
        std::string protocol = "SERVER_PROTOCOL=HTTP/1.1";
        envp.push_back(const_cast <char *> ( protocol.c_str()));
        std::string length = "CONTENT_LENGTH=" + (request.getRequestBody().size());
        envp.push_back(const_cast <char *> ( length.c_str()));
        req >> type;
        std::string pathInfo = "PATH_INFO=" + type;
        envp.push_back(const_cast <char *> ( pathInfo.c_str()));

        
        
        // envp.push_back(const_cast <char *>(envVar1.c_str()));
        // envp.push_back(const_cast <char *>(envVar2.c_str()));
        //envp.push_back(NULL);
        //env["CONTENT_TYPE"] = request->getHeader("Content-Type");
        //env["GATEWAY_INTERFACE"] = "CGI/1.1";
        //env["REQUEST_METHOD"] = type;
        //env["QUERY_STRING"] = request->getQueryString();
        //env["REMOTE_ADDR"] = request->getRemoteAddr();
        // std::cout << "REQUEST - " << type << std::endl;
        //env["SCRIPT_NAME"] = cgiScriptPath;
        //env["SERVER_NAME"] = request->getServerName();
        //env["SERVER_PORT"] = (std::string)(request->getServerPort());
        //env["SERVER_PROTOCOL"] = "HTTP/1.1";
        //env["SERVER_SOFTWARE"] = "webserv/1.0";
        //std::cout << envVar << std::endl;
        // Convert environment variables to char* array
        //std::vector<char*> envp;
        // for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); it++) {
        //     std::string envVar = it->first;
        //     envVar += "=";
        //     envVar += it->second;
        //     envp.push_back(const_cast <char *> ( envVar.c_str()));
        //     std::cout << envVar << std::endl;
        // }
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