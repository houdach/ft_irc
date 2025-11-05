#ifndef BOT_HPP
#define BOT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
#include <cstdlib>
#include "../../include/Server.hpp"
#include "../../include/Client.hpp"
#include "../../include/Channel.hpp"

class Server;

class Bot
{
private:
    std::string         _nickname;
    std::string         _username;
    std::string         _realname;
    bool                _active;
    Server*             _server;
    Client*             _client;
    std::vector<std::string> _jokes;

public:
    Bot(Server* server, const std::string& nickname = "HelperBot");
    ~Bot();

    void start();
    void stop();

    void onMessage(const std::string& channel, const std::string& sender, const std::string& message);
    void handleCommand(const std::string& channel, const std::string& sender, const std::string& command);
    void sendMessage(const std::string& channel, const std::string& text);

    std::string getTime() const;
    std::string getHelpMessage() const;
};

#endif
