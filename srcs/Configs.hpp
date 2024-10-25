
#ifndef CONFIGS_HPP
# define CONFIGS_HPP

#include "webserv.hpp"

typedef struct s_config
{
    int IP;
    std::vector<int> ports;
    std::string hostname;
}   t_config;

class Configs
{
    private:
        std::vector<t_config> configs;
    
    public:
        Configs();
        ~Configs();

};


#endif