/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiManagement.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masoares <masoares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 19:10:43 by masoares          #+#    #+#             */
/*   Updated: 2024/11/17 12:00:11 by masoares         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

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
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        const char * args[3] = {info._cgiPath.c_str(),  file.c_str(), NULL};
        if(execve(args[0], const_cast<char* const*>(args), 0) == -1)
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