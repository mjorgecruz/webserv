
#ifndef SERVER_HPP
# define SERVER_HPP

#include "general.hpp"
#include "Location.hpp"

class Location;

class Server : public Socket
{
    private:
        std::string host;
        int ports;
        std::vector<std::string> hostname;

        std::vector<std::string> index;
        std::map<int, std::string> errorPages;

        std::vector<Location> locations;
    
        ServerConfig(const ServerConfig & src);
        ServerConfig &operator=(const ServerConfig &src);
    public:
        ServerConfig();
        ~ServerConfig();
        void setConfigs(std::string path);
        Location operator[](int num) const;
        size_t locationsSize() const;
        

        std::string getHost();
        int getPorts();
        std::vector<std::string> getHostname();

        std::vector<std::string> getIndex();
        std::map<int, std::string> getErrorPages();

        std::vector<Location> getLocations();
    
};


#endif