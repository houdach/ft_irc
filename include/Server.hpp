#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <string>
#include "Client.hpp"
#include "Channel.hpp"

class Server {
private:
    int _port;                              // Port number to listen on
    int _socketFd;                          // Listening socket FD
    std::map<int, Client*> _clients;        // fd -> Client
    std::map<std::string, Channel*> _channels; // name -> Channel
    std::string _password;                  // Optional server password

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
