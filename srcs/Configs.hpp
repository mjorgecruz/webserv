
#ifndef CONFIGS_HPP
# define CONFIGS_HPP

#include "webserv.hpp"

typedef struct s_config
{
    std::string host;
    int ports;
    std::string hostname;

    std::vector<std::string> index;
    std::string errorPage;
}   t_config;

class Configs
{
    private:
        std::vector<t_config> configs;
    
        Configs(const Configs & src);
        Configs &operator=(const Configs &src);
    public:
        Configs();
        ~Configs();
        void setConfigs(std::string path);
        void addConfig(t_config &configs);
        t_config operator[](int num) const;
        size_t configSize() const;
};


#endif