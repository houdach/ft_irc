#include "include/Server.hpp"
#include "include/Request.hpp"
#include <sstream>
#include <vector>
#include <fcntl.h>

Server::Server() {}

Server::~Server() 
{
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) 
    {
        close(it->first);
        delete it->second;
    }
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) 
    {
        delete it->second;
    }
    if (socketFd >= 0)
        close(socketFd);
}

void Server::init(int port, const std::string& password)
{
    this->port = port;
    this->password = password;
    int opt = 1;
    struct sockaddr_in addr;
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0)
        throw ServerException("Failed to create socket");
    if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw ServerException("Failed to set SO_REUSEADDR");
    fcntl(socketFd, F_SETFL, O_NONBLOCK);
    if (bind(socketFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::stringstream ziko;
        ziko << "Failed to bind in  port: " << port;
        throw ServerException(ziko.str());
    }
    if (listen(socketFd, 10) < 0)
        throw ServerException("Failed to listen");

    std::cout << "Server listening on port " << port
    << " (fd=" << socketFd << ")" << std::endl;
}

void Server::run()
{
    std::cout << "Server running on port " << port << std::endl;
    struct pollfd Serverpoll;
    Serverpoll.fd = socketFd;
    Serverpoll.events = POLLIN;
    Serverpoll.revents = 0;
    pollfds.push_back(Serverpoll);
    while (true)
    {
        int ret = poll(pollfds.data(), pollfds.size(), -1);
        if (ret < 0) break;
        if (pollfds[0].revents & POLLIN)
        {
            int clientFd = accept(socketFd, nullptr, nullptr);
            if (clientFd >= 0)
            {
                struct pollfd clientPoll;
                clientPoll.fd = clientFd;
                clientPoll.events = POLLIN;
                pollfds.push_back(clientPoll);
                std::cout << "New client connected: " << clientFd << std::endl;
            }
        }
        for (size_t i = 1; i < pollfds.size(); ++i)
        {
            if (pollfds[i].revents & POLLIN)
            {
                char buffer[1024] = {0};
                int bytes = recv(pollfds[i].fd, buffer, sizeof(buffer), 0);
                if (bytes > 0)
                    std::cout << "Client " << pollfds[i].fd << " says: " << buffer << std::endl;
                else {
                    close(pollfds[i].fd);
                    pollfds.erase(pollfds.begin() + i);
                    --i;
                }
            }
        }
    }
}