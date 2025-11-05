#ifndef DCCTRANSFER_HPP
#define DCCTRANSFER_HPP

#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

class DCCTransfer
{
private:
    int         _sockfd;
    std::string _filename;
    std::string _targetNick;
    std::string _mode;
    size_t      _filesize;
    int         _port;
    std::string _ip;

public:
    DCCTransfer(const std::string& mode,
                const std::string& targetNick,
                const std::string& filename,
                const std::string& ip,
                int port,
                size_t filesize);

    ~DCCTransfer();

    bool start();
    bool sendFile();
    bool receiveFile();
};

#endif
