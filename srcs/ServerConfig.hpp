
#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include "webserv.hpp"

typedef struct s_config
{
    std::string host;
    int ports;
    std::string hostname;

    std::vector<std::string> index;
    std::string errorPage;
    std::map <std::string, std::string> locations;
}   t_config;

class ServerConfig
{
    private:
        std::vector<t_config *> configs;
    
        ServerConfig(const ServerConfig & src);
        ServerConfig &operator=(const ServerConfig &src);
    public:
        ServerConfig();
        ~ServerConfig();
        void setConfigs(std::string path);
        void addConfig(t_config *ServerConfig);
        t_config *operator[](int num) const;
        size_t configSize() const;
};


#endif