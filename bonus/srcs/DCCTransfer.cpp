#include "../includes/DCCTransfer.hpp"
#include <sys/socket.h>
#include <fcntl.h>
#include <cstring>

DCCTransfer::DCCTransfer(const std::string& mode,
                         const std::string& targetNick,
                         const std::string& filename,
                         const std::string& ip,
                         int port,
                         size_t filesize)
    : _sockfd(-1), _filename(filename), _targetNick(targetNick),
      _mode(mode), _filesize(filesize), _port(port), _ip(ip)
{
    (void) _filesize;
}

DCCTransfer::~DCCTransfer()
{
    if (_sockfd != -1)
        close(_sockfd);
}

bool DCCTransfer::start()
{
    if (_mode == "SEND")
        return sendFile();
    else if (_mode == "RECEIVE")
        return receiveFile();
    return false;
}

bool DCCTransfer::sendFile()
{
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0)
    {
        perror("socket");
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(_port);

    if (bind(listenFd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        close(listenFd);
        return false;
    }

    listen(listenFd, 1);
    std::cout << "📤 DCC SEND waiting for connection on port " << _port << "..." << std::endl;

    sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    int connFd = accept(listenFd, (struct sockaddr*)&clientAddr, &len);
    if (connFd < 0)
    {
        perror("accept");
        close(listenFd);
        return false;
    }
    close(listenFd);

    std::ifstream file(_filename.c_str(), std::ios::binary);
    if (!file)
    {
        std::cerr << "Error opening file: " << _filename << std::endl;
        close(connFd);
        return false;
    }

    char buffer[4096];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0)
    {
        ssize_t sent = send(connFd, buffer, file.gcount(), 0);
        if (sent < 0)
        {
            perror("send");
            close(connFd);
            return false;
        }
    }

    std::cout << "✅ File " << _filename << " sent successfully." << std::endl;
    close(connFd);
    return true;
}

bool DCCTransfer::receiveFile()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    inet_pton(AF_INET, _ip.c_str(), &addr.sin_addr);

    std::cout << "📥 Connecting to " << _ip << ":" << _port << " for DCC..." << std::endl;

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        close(sock);
        return false;
    }

    std::ofstream file(_filename.c_str(), std::ios::binary);
    if (!file)
    {
        std::cerr << "Error creating file: " << _filename << std::endl;
        close(sock);
        return false;
    }

    char buffer[4096];
    ssize_t bytes;
    while ((bytes = recv(sock, buffer, sizeof(buffer), 0)) > 0)
    {
        file.write(buffer, bytes);
    }

    std::cout << "✅ File received: " << _filename << std::endl;
    close(sock);
    return true;
}
