#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <string>
#include <iostream>
#include <cstdlib>      
#include <cstring>     
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>  
#include <unistd.h>  
#include "Client.hpp"
#include "Channel.hpp"
#include <stdexcept>
#include <poll.h>

class ServerException : public std::runtime_error {
public:
    ServerException(const std::string& msg)
        : std::runtime_error(msg) {}
};

class Server {
private:
    int port;                              // Port number to listen on
    int socketFd;                          // Listening socket FD
    std::map<int, Client*> clients;        // fd -> Client
    std::map<std::string, Channel*> channels; // name -> Channel
    std::string password;                  // Optional server password

public:
    Server();
    ~Server();

    void init(int port, const std::string& password);
    void run();

    // Client management
    void addClient(int fd);
    void removeClient(int fd);

    // Accessors
    Client* getClient(int fd);
    Channel* getChannel(const std::string& name);
};

#endif
