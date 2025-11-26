// DCCTransfer.hpp
#ifndef DCCTRANSFER_HPP
#define DCCTRANSFER_HPP

#include <string>
#include <fstream>
#include <iostream>

class DCCTransfer
{
private:
    std::string _filename;        // Source file
    std::string _targetNick;      // Receiver nickname
    std::string _filesizeStr;     // For notifications

public:
    DCCTransfer(const std::string& targetNick,
                const std::string& filename,
                const std::string& filesizeStr);

    ~DCCTransfer() {}

    void start(); // Internal copy transfer
};

#endif
