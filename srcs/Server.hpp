
#ifndef SERVER_HPP
# define SERVER_HPP

#include "general.hpp"
#include "Location.hpp"

class Location;

class Server : public Socket
{
    private:
        std::string _host;
        int _ports;
        std::string _root;
        std::vector<std::string> _hostname;
        std::vector<std::string> _index;
        std::map<int, std::string> _errorPages;
        std::map<std::string, Location *> _locations;
        std::vector<std::string> _allowedMethods;
        long _maxBodySize;
    
        Server(const Server & src);

    public:
        Server();
        ~Server();
        void setConfigs(std::string path);
        Location operator[](int num) const;
        size_t locationsSize() const;
        

        std::string getHost();
        int getPorts();
        std::vector<std::string> getHostname();
        std::vector<std::string> getIndex();
        std::map<int, std::string> getErrorPages();
        std::map<std::string, Location *> getLocations();
        std::vector<std::string> getAllowedMethods();
        std::string getRoot();

        void setHost(std::string host);
        void setPorts(int port);
        void setHostname( std::vector<std::string> hostnames);
        void setIndex(std::vector<std::string> index);
        void setErrorPages(std::map<int, std::string> errorPages);

        void addLocations(std::string path, Location *locations);
        void addAllowedMethods(std::string method);
        void setRoot(std::string root);
        
        void serverChecker(std::string &line, std::ifstream &file);
        void serverKeywords(std::string key, std::string &line);
        void printConfig() const;
};


#endif