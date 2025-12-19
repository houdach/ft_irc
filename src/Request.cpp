#include "../include/Request.hpp"
#include <sstream>

Request::Request() : _prefix(""), _command("") 
{
}

Request::~Request() 
{
}

Request Request::parse(const std::string& line) 
{
    Request req;
    std::string temp = line;
    
    while (!temp.empty() && (temp[temp.length() - 1] == ' ' || 
                             temp[temp.length() - 1] == '\t' ||
                             temp[temp.length() - 1] == '\r' ||
                             temp[temp.length() - 1] == '\n'))
        temp.erase(temp.length() - 1);
    
    if (temp.empty())
        return req;
    
    size_t pos = 0;
    if (temp[0] == ':') 
    {
        pos = temp.find(' ');
        if (pos == std::string::npos) 
        {
            req._prefix = temp.substr(1);
            return req;
        }
        req._prefix = temp.substr(1, pos - 1);
        temp = temp.substr(pos + 1);
    }
    
    pos = temp.find(' ');
    if (pos == std::string::npos) 
    {
        req._command = temp;
        return req;
    }
    req._command = temp.substr(0, pos);
    temp = temp.substr(pos + 1);
    
    while (!temp.empty()) 
    {
        while (!temp.empty() && temp[0] == ' ')
            temp.erase(0, 1);
        
        if (temp.empty())
            break;
        
        if (temp[0] == ':') 
        {
            req._params.push_back(temp.substr(1));
            break;
        }
        
        pos = temp.find(' ');
        if (pos == std::string::npos) 
        {
            req._params.push_back(temp);
            break;
        }
        req._params.push_back(temp.substr(0, pos));
        temp = temp.substr(pos + 1);
    }
    
    return req;
}

std::string Request::getCommand() const 
{
    return _command;
}

const std::vector<std::string>& Request::getParams() const 
{
    return _params;
}

std::string Request::getPrefix() const 
{
    return _prefix;
}
