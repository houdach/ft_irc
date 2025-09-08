#include "../include/Server.hpp"

Server::Server() {}
Server::~Server() {}

void Server::init(int port, const std::string& password)
{
    this->port = port;
    this->password = password;
    int opt = 1;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;         
    addr.sin_port = htons(port);      
    addr.sin_addr.s_addr = INADDR_ANY;
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
   if (socketFd < 0)
     throw ServerException("Failed to create socket");
   if (setsockopt(this->socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw ServerException("Failed to set SO_REUSEADDR");
   if (bind(this->socketFd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        throw ServerException("Bind failed on port " + std::to_string(port));

    if (listen(this->socketFd, 10) < 0)
        throw ServerException("Listen failed");
    
    printf("[+] Server listening on port %d (fd=%d)\n", port, socketFd);

}
void Server::run()
{
        std::cout << "Server running on port " << port << "..." << std::endl;

}
