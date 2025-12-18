#include "../includes/bot.hpp"
#include <ctime>
#include <cstdlib>

Bot::Bot(Server* server, const std::string& nickname)
    : _nickname(nickname), _username("IRC Helper Bot"),
      _realname("MyBot"), _active(false), _server(server)
{
    std::cout << "🤖 Bot " << _nickname << " created." << std::endl;
    _jokes.push_back("Why do programmers prefer dark mode? Because light attracts bugs!");
    _jokes.push_back("I told my compiler a joke... but it gave me a warning instead.");
    _jokes.push_back("There are 10 kinds of people in the world: those who understand binary and those who don't.");
    _jokes.push_back("A SQL query walks into a bar, walks up to two tables and asks: ‘Can I join you?’");
}

Bot::~Bot()
{
    if (_active)
        stop();
    std::cout << "🤖 Bot " << _nickname << " disconnected." << std::endl;
}

void Bot::start()
{
    if (_active)
        return;

    _active = true;
    _client = new Client(-1); 
    _client->setNick(_nickname);
    _client->setRegistered(true);
  _server->registerInternalClient(-1, _client);


    std::cout << "🤖 Bot started as internal client." << std::endl;
}

void Bot::stop()
{
    if (!_active)
        return;
    _active = false;
    delete _client;
}

void Bot::onMessage(const std::string& channel, const std::string& sender, const std::string& message)
{
    if (!_active)
        return;

    if (message.empty())
        return;

    if (message[0] == '!')
        handleCommand(channel, sender, message.substr(1));
}

void Bot::handleCommand(const std::string& channel, const std::string& sender, const std::string& command)
{
     (void)sender; 
    if (command == "help")
        sendMessage(channel, getHelpMessage());
    else if (command == "time")
        sendMessage(channel, "🕒 Current server time: " + getTime());
    else if (command == "users")
    {
        Channel* ch = _server->getChannel(channel);
        if (!ch)
        {
            sendMessage(channel, "No such channel.");
            return;
        }
        std::stringstream ss;
        ss << "👥 Users in " << channel << ": ";
        std::vector<Client*> users = ch->getUsers();
        for (size_t i = 0; i < users.size(); ++i)
        {
            ss << users[i]->getNick();
            if (i < users.size() - 1)
                ss << ", ";
        }
        sendMessage(channel, ss.str());
    }
    else if (command == "joke")
    {
        size_t idx = rand() % _jokes.size();
        sendMessage(channel, "😂 " + _jokes[idx]);
    }
    else if (command == "info")
    {
        sendMessage(channel, "I'm " + _nickname + ", your friendly IRC bot. Type !help for commands.");
    }
    else
    {
        sendMessage(channel, "❓ Unknown command. Type !help for help.");
    }
}

void Bot::sendMessage(const std::string& channel, const std::string& text)
{
    Channel* ch = _server->getChannel(channel);
    if (!ch)
    {
        std::cerr << "Bot tried to send message to non-existent channel: " << channel << std::endl;
        return;
    }
    std::string msg = ":" + _nickname + " PRIVMSG " + channel + " :" + text;
    ch->broadcast(msg, _client);
}

std::string Bot::getTime() const
{
    std::time_t now = std::time(NULL);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

std::string Bot::getHelpMessage() const
{
    return "Available commands: !help, !time, !users, !joke, !info";
}
