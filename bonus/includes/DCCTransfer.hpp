// DCCTransfer.hpp
#ifndef DCCTRANSFER_HPP
#define DCCTRANSFER_HPP

#include <string>
#include <fstream>
#include <iostream>

class DCCTransfer
{
private:
    std::string _filename;       
    std::string _targetNick;  
    std::string _filesizeStr;    

public:
    DCCTransfer(const std::string& targetNick,
                const std::string& filename,
                const std::string& filesizeStr);

    ~DCCTransfer() {}

    void start();
};

#endif
