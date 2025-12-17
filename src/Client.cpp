#include "../include/Client.hpp"

Client::Client(int fd) 
    : _fd(fd), _nick(""), _username(""), _realname(""), _registered(false), _passwordAuthenticated(false), _bufferIn("")
{
}

Client::~Client() 
{
}

int Client::getFd() const 
{
    return _fd;
}

std::string Client::getNick() const 
{
    return _nick;
}

void Client::setNick(const std::string& nick) 
{
    _nick = nick;
}

std::string Client::getUsername() const
{
    return _username;
}

void Client::setUsername(const std::string& username)
{
    _username = username;
}

std::string Client::getRealname() const
{
    return _realname;
}

void Client::setRealname(const std::string& realname)
{
    _realname = realname;
}

bool Client::isRegistered() const 
{
    return _registered;
}

void Client::setRegistered(bool status) 
{
    _registered = status;
}

bool Client::isPasswordAuthenticated() const 
{
    return _passwordAuthenticated;
}

void Client::setPasswordAuthenticated(bool status) 
{
    _passwordAuthenticated = status;
}

void Client::appendIncoming(const std::string& data) 
{
    _bufferIn += data;
}

std::string Client::popLine() 
{
    size_t pos = _bufferIn.find("\r\n");
    if (pos == std::string::npos) 
    {
        pos = _bufferIn.find("\n");
        if (pos == std::string::npos)
            return "";
    }
    
    std::string line = _bufferIn.substr(0, pos);
    _bufferIn.erase(0, pos + ((_bufferIn[pos] == '\r') ? 2 : 1));
    return line;
}

void Client::queueMessage(const std::string& msg) 
{
    _bufferOut.push_back(msg);
}

std::string Client::nextMessage() 
{
    if (_bufferOut.empty())
        return "";
    std::string msg = _bufferOut.front();
    _bufferOut.erase(_bufferOut.begin());
    return msg;
}
