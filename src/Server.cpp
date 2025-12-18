#include "../include/Server.hpp"
#include "../include/Request.hpp"
#include <sstream>
#include <vector>
#include <fcntl.h>
#include "../bonus/includes/bot.hpp"

static void sendToClient(int fd, const std::string& msg);
static void sendNumeric(int fd, int code, const std::string& nick, const std::string& msg);

Server::Server() {}

Server::~Server() 
{
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) 
    {
        close(it->first);
        delete it->second;
    }
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) 
    {
        delete it->second;
    }   
    if (bot)
        delete bot;
    if (socketFd >= 0)
        close(socketFd);
}

void Server::init(int port, const std::string& password)
{
    this->port = port;
    this->password = password;
    bot = new Bot(this); 
    bot->start(); 
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
    {
        std::stringstream ss;
        ss << "Bind failed on port " << port;
        throw ServerException(ss.str());
    }
    if (listen(this->socketFd, 10) < 0)
        throw ServerException("Listen failed");
    
    fcntl(socketFd, F_SETFL, O_NONBLOCK);
    
    std::cout << "[+] Server listening on port " << port
    << " (fd=" << socketFd << ")" << std::endl;
}

void Server::addClient(int fd) 
{
    Client* client = new Client(fd);
    clients[fd] = client;
    fcntl(fd, F_SETFL, O_NONBLOCK);
    std::cout << "[+] New client connected (fd=" << fd << ")" << std::endl;
    sendToClient(fd, "Connection established. Authentication is required before receiving your full welcome. Start with PASS.");
} 

void Server::removeClient(int fd) 
{
    std::map<int, Client*>::iterator it = clients.find(fd);
    if (it != clients.end()) 
    {
        std::cout << "[-] Client disconnected (fd=" << fd << ")" << std::endl;
        for (std::map<std::string, Channel*>::iterator chIt = channels.begin(); 
             chIt != channels.end(); ++chIt) 
            chIt->second->removeUser(it->second);
        delete it->second;
        clients.erase(it);
        close(fd);
    }
}

Client* Server::getClient(int fd) 
{
    std::map<int, Client*>::iterator it = clients.find(fd);
    if (it != clients.end())
        return it->second;
    return NULL;
}

Client* Server::getClientByNick(const std::string& nick) 
{
    for (std::map<int, Client*>::iterator it = clients.begin(); 
         it != clients.end(); ++it) 
    {
        if (it->second->getNick() == nick)
            return it->second;
    }
    return NULL;
}

Channel* Server::getChannel(const std::string& name) 
{
    std::map<std::string, Channel*>::iterator it = channels.find(name);
    if (it != channels.end())
        return it->second;
    return NULL;
}

std::string Server::getPassword() const 
{
    return password;
}

static void sendToClient(int fd, const std::string& msg) 
{
    std::string fullMsg = msg;
    if (fullMsg.find("\r\n") == std::string::npos)
        fullMsg += "\r\n";
    send(fd, fullMsg.c_str(), fullMsg.length(), 0);
}

static void sendNumeric(int fd, int code, const std::string& nick, const std::string& msg) 
{
    std::stringstream ss;
    ss << ":server " << code << " " << (nick.empty() ? "*" : nick) << " " << msg;
    sendToClient(fd, ss.str());
}

void handlePass(Server* server, Client* client, const Request& req) 
{
    if (client->isPasswordAuthenticated()) 
    {
        sendToClient(client->getFd(), ":server NOTICE * :You have already entered a valid password");
        return;
    }
    
    if (req.getParams().empty()) 
    {
        sendNumeric(client->getFd(), 461, client->getNick(), "PASS :Not enough parameters");
        return;
    }
    if (req.getParams()[0] == server->getPassword()) 
    {
        client->setPasswordAuthenticated(true);
        std::cout << "Password correct" << std::endl;
        sendToClient(client->getFd(), "Password correct, Please enter your nickname using the NICK command.");
    } 
    else 
    {
        sendNumeric(client->getFd(), 464, client->getNick(), ":Password incorrect");
        sendToClient(client->getFd(), ":server NOTICE * :Please enter a valid password");
    }
}

void handleNick(Server* server, Client* client, const Request& req) 
{
    (void)server;
    if (req.getParams().empty()) 
    {
        sendNumeric(client->getFd(), 431, client->getNick(), ":No nickname given");
        return;
    }

    if (!client->isPasswordAuthenticated()) 
    {
        sendNumeric(client->getFd(), 464, client->getNick(), ":Password required");
        return;
    }

    std::string newNick = req.getParams()[0];
    std::string oldNick = client->getNick();
    Client* existing = server->getClientByNick(newNick);
    if (existing && existing != client)
    {
        sendNumeric(client->getFd(), 433, client->getNick(), newNick + " :Nickname is already in use");
        return;
    }
    client->setNick(newNick);
    
    if (!oldNick.empty()) 
    {
        std::stringstream ss;
        ss << ":" << oldNick << " NICK " << newNick;
        sendToClient(client->getFd(), ss.str());
        sendToClient(client->getFd(), ":server NOTICE " + newNick + " :✅ Your nickname has been successfully updated to " + newNick);
    }
    else
    {
        sendToClient(client->getFd(), ":server NOTICE " + newNick + " :👋 Nickname set! Please complete your registration using the USER command.");
        sendToClient(client->getFd(), "USER <nickname> 0 *:<fullname>");

    }
}

void handleUser(Server* server, Client* client, const Request& req) 
{
    (void)server;
    if (req.getParams().size() < 3) 
    {
        sendNumeric(client->getFd(), 461, client->getNick(), "USER :Not enough parameters");
        return;
    }

    if (!client->isPasswordAuthenticated()) 
    {
        sendNumeric(client->getFd(), 464, client->getNick(), ":Password required");
        return;
    }
    client->setUsername(req.getParams()[0]);
    client->setRealname(req.getParams()[2]);

    if (!client->isRegistered() && !client->getNick().empty() && !client->getUsername().empty()) 
    {
        client->setRegistered(true);
        sendNumeric(client->getFd(), 001, client->getNick(), ":Welcome to the IRC Network");
        sendNumeric(client->getFd(), 002, client->getNick(), ":Your host is ircserv");
        sendNumeric(client->getFd(), 003, client->getNick(), ":This server was created recently");
    }
}

void handleJoin(Server* server, Client* client, const Request& req) 
{
    if (!client->isRegistered()) 
    {
        sendNumeric(client->getFd(), 451, client->getNick(), ":You have not registered");
        return;
    }
    
    if (req.getParams().empty()) 
    {
        sendNumeric(client->getFd(), 461, client->getNick(), "JOIN :Not enough parameters");
        return;
    }
    
    std::string channelName = req.getParams()[0];
    if (channelName.empty() || channelName[0] != '#') 
    {
        sendNumeric(client->getFd(), 403, client->getNick(), channelName + " :No such channel");
        return;
    }
    
    Channel* channel = server->getChannel(channelName);
    if (!channel) 
    {
        channel = new Channel(channelName);
        server->channels[channelName] = channel;
        if (req.getParams().size() > 1 && !req.getParams()[1].empty())
            channel->setKey(req.getParams()[1]);
        channel->addUser(client);
        channel->addOperator(client);
    } 
    else 
    {
        if (channel->getInviteOnly() && !channel->isOperator(client) && !channel->isInvited(client->getNick()))
        {
            sendNumeric(client->getFd(), 473, client->getNick(), channelName + " :Cannot join channel (+i)");
            return;
        }
        if (!channel->getKey().empty())
        {
            std::string providedKey = (req.getParams().size() > 1) ? req.getParams()[1] : std::string();
            if (providedKey != channel->getKey())
            {
                sendNumeric(client->getFd(), 475, client->getNick(), channelName + " :Cannot join channel (secret code needed)");
                return;
            }
        }

        if (channel->isFull()) 
        {
            sendNumeric(client->getFd(), 471, client->getNick(), channelName + " :Cannot join channel (+l)");
            return;
        }

        channel->addUser(client);
        if (channel->isInvited(client->getNick()))
            channel->removeInvite(client->getNick());
    }
    
    std::stringstream joinMsg;
    joinMsg << "🎉 Welcome to the community, " << client->getNick() << "! You have joined the channel " << channelName;
    sendToClient(client->getFd(), joinMsg.str());
    
    std::stringstream brodMsg;
    brodMsg << "The user "<< client->getNick() << " joined " << channelName;
    channel->broadcast(brodMsg.str(), client);
    
    if (!channel->getTopic().empty()) 
    {
        sendNumeric(client->getFd(), 332, client->getNick(), 
                    channelName + " :" + channel->getTopic());
    }
    
    std::stringstream names;
    std::vector<Client*> users = channel->getUsers();
    for (size_t i = 0; i < users.size(); i++) 
    {
        if (channel->isOperator(users[i]))
            names << "@";
        names << users[i]->getNick();
        if (i < users.size() - 1)
            names << " ";
    }
}

void Server::handlePrivmsg(Client* client, const Request& req) 
{
    if (!client->isRegistered()) 
    {
        sendNumeric(client->getFd(), 451, client->getNick(), ":You have not registered");
        return;
    }
    
    if (req.getParams().size() < 2) 
    {
        sendNumeric(client->getFd(), 411, client->getNick(), ":No recipient given (PRIVMSG)");
        return;
    }
    
    std::string target = req.getParams()[0];
    std::string message;
    if (req.getParams().size() >= 2)
    {
        message = req.getParams()[1];
        for (size_t i = 2; i < req.getParams().size(); ++i)
            message += " " + req.getParams()[i];
    }
    
    std::stringstream msg;
    msg << ":" << client->getNick() << " PRIVMSG " << target << " :" << message;
    
        if (message.find("DCC SEND") != std::string::npos)
        {
            std::cout << "📡 DCC SEND detected: " << message << std::endl;

            std::stringstream ss(message);
            std::string dcc, cmd, filename;
            unsigned long ipInt;
            int port;
            size_t filesize;
            ss >> dcc >> cmd >> filename >> ipInt >> port >> filesize;

            Client* targetClient = this->getClientByNick(target);

            if (!targetClient || client == targetClient)
            {
                std::cerr << "ERROR: Invalid target client for DCC SEND.\n";
                return;
            }
            std::stringstream ss2;
            ss2 << filesize;
            std::string filesizeStr = ss2.str();
            DCCTransfer recvTransfer(targetClient->getNick(), filename, filesizeStr);
            recvTransfer.start();
            std::string senderNotice = "\033[32mFile transfer to " + targetClient->getNick() + " started!\033[0m\r\n";
            send(targetClient->getFd(), senderNotice.c_str(), senderNotice.size(), 0);
            sendToClient(targetClient->getFd(), "File recieved successfully");
            return;
        }


    if (target[0] == '#') 
    {
        Channel* channel = this->getChannel(target);
        if (!channel) 
        {
            sendNumeric(client->getFd(), 403, client->getNick(), target + " :No such channel");
            return;
        }
        
        if (!channel->hasUser(client)) 
        {
            sendNumeric(client->getFd(), 442, client->getNick(), target + " :You're not on that channel");
            return;
        }
        
        channel->broadcast(msg.str(), client);
        if (bot && target[0] == '#')
            bot->onMessage(target, client->getNick(), message);

    } 
    else 
    {
        Client* targetClient = this->getClientByNick(target);
        if (!targetClient) 
        {
            sendNumeric(client->getFd(), 401, client->getNick(), target + " :No such nick/channel");
            return;
        }
        sendToClient(targetClient->getFd(), msg.str());
    }
}

void handleNotice(Server* server, Client* client, const Request& req)
{
    if (!client->isRegistered())
        return;

    if (req.getParams().size() < 2)
        return;

    std::string target = req.getParams()[0];
    std::string message;
    if (req.getParams().size() >= 2)
    {
        message = req.getParams()[1];
        for (size_t i = 2; i < req.getParams().size(); ++i)
            message += " " + req.getParams()[i];
    }

    std::stringstream msg;
    msg << ":" << client->getNick() << " NOTICE " << target << " :" << message;

    if (target.empty())
        return;

    if (target[0] == '#')
    {
        Channel* channel = server->getChannel(target);
        if (!channel)
            return;
        if (!channel->hasUser(client))
            return;
        channel->broadcast(msg.str(), client);
    }
    else
    {
        Client* targetClient = server->getClientByNick(target);
        if (!targetClient)
            return;
        sendToClient(targetClient->getFd(), msg.str());
    }
}

void handlePart(Server* server, Client* client, const Request& req) 
{
    if (!client->isRegistered()) 
    {
        sendNumeric(client->getFd(), 451, client->getNick(), ":You have not registered");
        return;
    }
    
    if (req.getParams().empty()) 
    {
        sendNumeric(client->getFd(), 461, client->getNick(), "PART :Not enough parameters");
        return;
    }
    
    std::string channelName = req.getParams()[0];
    std::string reason = req.getParams().size() > 1 ? req.getParams()[1] : "Leaving";
    if (channelName.empty() || channelName[0] != '#') 
    {
        sendNumeric(client->getFd(), 403, client->getNick(), channelName + " :No such channel");
        return;
    }
    
    Channel* channel = server->getChannel(channelName);
    if (!channel) 
    {
        sendNumeric(client->getFd(), 403, client->getNick(), channelName + " :No such channel");
        return;
    }
    
    if (!channel->hasUser(client)) 
    {
        sendNumeric(client->getFd(), 442, client->getNick(), channelName + " :You're not on that channel");
        return;
    }
    std::stringstream partMsg;
    partMsg << ":" << client->getNick() << " PART " << channelName << " :" << reason;
    channel->broadcast(partMsg.str(), NULL); 
    sendToClient(client->getFd(), partMsg.str());
    
    channel->removeUser(client);
    if (channel->getUserCount() == 0) 
    {
        server->channels.erase(channelName);
        delete channel;
    }
}

void handleKick(Server* server, Client* client, const Request& req) 
{
    if (!client->isRegistered()) 
    {
        sendNumeric(client->getFd(), 451, client->getNick(), ":You have not registered");
        return;
    }
    
    if (req.getParams().size() < 3) 
    {
        sendNumeric(client->getFd(), 461, client->getNick(), "KICK :Not enough parameters");
        return;
    }
    
    std::string serverName = req.getParams()[0];
    std::string channelName = req.getParams()[1];
    std::string targetNick = req.getParams()[2];
    std::string reason = req.getParams().size() > 2 ? req.getParams()[3] : client->getNick();
    
    Channel* channel = server->getChannel(channelName);
    if (!channel) 
    {
        sendNumeric(client->getFd(), 403, client->getNick(), channelName + " :No such channel");
        return;
    }
    
    if (!channel->isOperator(client)) 
    {
        sendNumeric(client->getFd(), 482, client->getNick(), channelName + " :You're not channel operator");
        return;
    }
    Client* targetClient = NULL;
    std::vector<Client*> users = channel->getUsers();
    for (size_t i = 0; i < users.size(); i++) 
    {
        if (users[i]->getNick() == targetNick) 
        {
            targetClient = users[i];
            break;
        }
    }
    
    if (!targetClient) 
    {
        sendNumeric(client->getFd(), 441, client->getNick(), 
                    targetNick + " " + channelName + " :They aren't on that channel");
        return;
    }
    if (targetClient == client)
    {
        sendToClient(client->getFd(), ":server NOTICE " + client->getNick() + " :You cannot kick yourself");
        return;
    }
    std::stringstream kickMsg;
    kickMsg << ":" << client->getNick() << " KICK " << channelName << " " 
            << targetNick << " :" << reason;
    channel->broadcast(kickMsg.str(), NULL);
    sendToClient(targetClient->getFd(), kickMsg.str());
    
    channel->removeUser(targetClient);
}

void handleTopic(Server* server, Client* client, const Request& req) 
{
    if (!client->isRegistered()) 
    {
        sendNumeric(client->getFd(), 451, client->getNick(), ":You have not registered");
        return;
    }
    
    if (req.getParams().empty()) 
    {
        sendNumeric(client->getFd(), 461, client->getNick(), "TOPIC :Not enough parameters");
        return;
    }
    
    std::string channelName = req.getParams()[0];
    Channel* channel = server->getChannel(channelName);
    
    if (!channel) 
    {
        sendNumeric(client->getFd(), 403, client->getNick(), channelName + " :No such channel");
        return;
    }
    
    if (req.getParams().size() == 1) 
    {
        if (channel->getTopic().empty()) 
        {
            sendNumeric(client->getFd(), 331, client->getNick(), channelName + " :No topic is set");
        } 
        else 
        {
            sendNumeric(client->getFd(), 332, client->getNick(), 
                        channelName + " :" + channel->getTopic());
        }
    } 
    else 
    {
        if (channel->getTopicRestricted() && !channel->isOperator(client)) 
        {
            sendNumeric(client->getFd(), 482, client->getNick(), 
                        channelName + " :You're not channel operator");
            return;
        }
        
        std::string newTopic = req.getParams()[1];
        channel->setTopic(newTopic);
        
        std::stringstream topicMsg;
        topicMsg << ":" << client->getNick() << " TOPIC " << channelName << " :" << newTopic;
        channel->broadcast(topicMsg.str(), NULL);
        sendToClient(client->getFd(), topicMsg.str());
    }
}

void handleInvite(Server* server, Client* client, const Request& req) 
{
    if (!client->isRegistered()) 
    {
        sendNumeric(client->getFd(), 451, client->getNick(), ":You have not registered");
        return;
    }
    
    if (req.getParams().size() < 2) 
    {
        sendNumeric(client->getFd(), 461, client->getNick(), "INVITE :Not enough parameters");
        return;
    }
    
    std::string targetNick = req.getParams()[0];
    std::string channelName = req.getParams()[1];
    
    Channel* channel = server->getChannel(channelName);
    if (!channel) 
    {
        sendNumeric(client->getFd(), 403, client->getNick(), channelName + " :No such channel");
        return;
    }
    
    if (!channel->hasUser(client)) 
    {
        sendNumeric(client->getFd(), 442, client->getNick(), channelName + " :You're not on that channel");
        return;
    }
    
    Client* targetClient = server->getClientByNick(targetNick);
    if (!targetClient)
    {
        sendNumeric(client->getFd(), 401, client->getNick(), targetNick + " :No such nick/channel");
        return;
    }

    channel->addInvite(targetNick);
    sendNumeric(client->getFd(), 341, client->getNick(), targetNick + " " + channelName);
    std::stringstream inviteMsg;
    inviteMsg << ":" << client->getNick() << " INVITE " << targetNick << " :" << channelName;
    sendToClient(targetClient->getFd(), inviteMsg.str());
}

void handleMode(Server* server, Client* client, const Request& req) 
{
    if (!client->isRegistered()) 
    {
        sendNumeric(client->getFd(), 451, client->getNick(), ":You have not registered");
        return;
    }
    
    if (req.getParams().empty()) 
    {
        sendNumeric(client->getFd(), 461, client->getNick(), "MODE :Not enough parameters");
        return;
    }
    
    std::string target = req.getParams()[0];
    
    if (target[0] == '#') 
    {
        Channel* channel = server->getChannel(target);
        if (!channel) 
        {
            sendNumeric(client->getFd(), 403, client->getNick(), target + " :No such channel");
            return;
        }
        
        if (req.getParams().size() == 1) 
        {
            std::stringstream modes;
            modes << "+";
            if (channel->getInviteOnly()) modes << "i";
            if (channel->getTopicRestricted()) modes << "t";
            if (channel->getUserLimit() > 0) modes << "l";
            if (!channel->getKey().empty()) modes << "k";
            
            sendNumeric(client->getFd(), 324, client->getNick(), target + " " + modes.str());
        } 
        else 
        {
            if (!channel->isOperator(client)) 
            {
                sendNumeric(client->getFd(), 482, client->getNick(), target + " :You're not channel operator");
                return;
            }
            
            std::string modeStr = req.getParams()[1];
            bool adding = true;
            
            for (size_t i = 0; i < modeStr.length(); i++) 
            {
                if (modeStr[i] == '+') 
                {
                    adding = true;
                } 
                else if (modeStr[i] == '-') 
                {
                    adding = false;
                } 
                else if (modeStr[i] == 'i') 
                {
                    channel->setInviteOnly(adding);
                } 
                else if (modeStr[i] == 't') 
                {
                    channel->setTopicRestricted(adding);
                } 
                else if (modeStr[i] == 'k') 
                {
                    if (adding && req.getParams().size() > 2) 
                    {
                        channel->setKey(req.getParams()[2]);
                    } 
                    else 
                    {
                        channel->setKey("");
                    }
                } 
                else if (modeStr[i] == 'l') 
                {
                    if (adding && req.getParams().size() > 2) 
                    {
                        int limit = std::atoi(req.getParams()[2].c_str());
                        channel->setUserLimit(limit);
                    } 
                    else 
                    {
                        channel->setUserLimit(-1);
                    }
                } 
                else if (modeStr[i] == 'o') 
                {
                    if (req.getParams().size() > 2) 
                    {
                        std::string targetNick = req.getParams()[2];
                        std::vector<Client*> users = channel->getUsers();
                        for (size_t j = 0; j < users.size(); j++) 
                        {
                            if (users[j]->getNick() == targetNick) 
                            {
                                if (adding) 
                                {
                                    channel->addOperator(users[j]);
                                } 
                                else 
                                {
                                    channel->removeOperator(users[j]);
                                }
                                break;
                            }
                        }
                    }
                }
            }
            std::stringstream modeMsg;
            modeMsg << ":" << client->getNick() << " MODE " << target << " " << modeStr;
            if (req.getParams().size() > 2) 
            {
                modeMsg << " " << req.getParams()[2];
            }
            channel->broadcast(modeMsg.str(), NULL);
            sendToClient(client->getFd(), modeMsg.str());
        }
    }
}

void Server::run()
{
    std::cout << "Server running on port " << port << "..." << std::endl;
        
    struct pollfd serverPoll;
    serverPoll.fd = socketFd;
    serverPoll.events = POLLIN;
    serverPoll.revents = 0;
    pollfds.push_back(serverPoll);
    
    while (true) 
    {
        pollfds.resize(1);
        
        for (std::map<int, Client*>::iterator it = clients.begin(); 
             it != clients.end(); ++it) 
        {
            struct pollfd clientPoll;
            clientPoll.fd = it->first;
            clientPoll.events = POLLIN;
            clientPoll.revents = 0;
            pollfds.push_back(clientPoll);
        }
        
        int ret = poll(&pollfds[0], pollfds.size(), -1);
        if (ret < 0) 
        {
            std::cerr << "poll() error" << std::endl;
            break;
        }
        
        if (pollfds[0].revents & POLLIN) 
        {
            struct sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            int clientFd = accept(socketFd, (struct sockaddr*)&clientAddr, &clientLen);
            if (clientFd >= 0)
                addClient(clientFd);
        }

        for (size_t i = 1; i < pollfds.size(); i++) 
        {
            if (pollfds[i].revents & POLLIN) 
            {
                int fd = pollfds[i].fd;
                Client* client = getClient(fd);
                
                if (!client)
                    continue;
                
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));
                int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
                
                if (bytes <= 0) 
                {
                    removeClient(fd);
                    continue;
                }
                
                buffer[bytes] = '\0';
                client->appendIncoming(std::string(buffer));
                std::string line;
                while (!(line = client->popLine()).empty()) 
                {
                    std::cout << "[" << fd << "] " << line << std::endl;
                    
                    Request req = Request::parse(line);
                    std::string cmd = req.getCommand();
                    
                    if (cmd == "PASS") 
                        handlePass(this, client, req);
                    else if (cmd == "NICK") 
                        handleNick(this, client, req); 
                    else if (cmd == "USER") 
                        handleUser(this, client, req); 
                    else if (cmd == "JOIN") 
                        handleJoin(this, client, req); 
                    else if (cmd == "PRIVMSG") 
                        handlePrivmsg(client, req); 
                    else if (cmd == "PART") 
                        handlePart(this, client, req); 
                    else if (cmd == "KICK") 
                        handleKick(this, client, req); 
                    else if (cmd == "INVITE") 
                        handleInvite(this, client, req); 
                    else if (cmd == "TOPIC") 
                        handleTopic(this, client, req); 
                    else if (cmd == "MODE") 
                        handleMode(this, client, req); 
                    else if (cmd == "PING") 
                    {
                        if (req.getParams().size() > 0)
                            sendToClient(fd, ":server PONG :" + req.getParams()[0]);
                        else
                            sendToClient(fd, ":server PONG :server");
                    }
                    else if (cmd == "NOTICE")
                        handleNotice(this, client, req);
                    else if (cmd == "QUIT") 
                    {
                        removeClient(fd);
                        break;
                    }
                }
            }
        }
    }
}
void Server::registerInternalClient(int fd, Client* client)
{
    clients[fd] = client;
}
