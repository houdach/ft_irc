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
#include "Request.hpp"
#include "../bonus/includes/DCCTransfer.hpp"

class Bot;
class ServerException : public std::runtime_error
{
    public:
        ServerException(const std::string& msg)
            : std::runtime_error(msg) {}
};

class Server
{
    private:
        int port;                              // Port number to listen on
        int socketFd;                          // Listening socket FD
        std::map<int, Client*> clients;        // fd -> Client
        std::string password;                  // Optional server password
        std::vector<struct pollfd> pollfds;
        Bot *bot;

    public:
        std::map<std::string, Channel*> channels; // name -> Channel
        
        Server();
        ~Server();

        void init(int port, const std::string& password);
        void run();

        // Client management
        void addClient(int fd);
        void removeClient(int fd);

        // Accessors
        Client* getClient(int fd);
        Client* getClientByNick(const std::string& nick);
        Channel* getChannel(const std::string& name);
        std::string getPassword() const;
        void handlePrivmsg(Client* client, const Request& req);
         void registerInternalClient(int fd, Client* client);
};

#endif
