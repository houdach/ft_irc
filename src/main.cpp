#include "../include/Server.hpp"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>\n";
        return 1;
    }

    int port = std::atoi(argv[1]);
    std::string password = argv[2];

    try 
    {
        Server server;
        server.init(port, password);
        server.run();
    } 
    catch (const ServerException& e) 
    {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }
}

