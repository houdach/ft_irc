#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include "Client.hpp"

class Channel {
private:
    std::string _name;             // Channel name (#room)
    std::string _topic;            // Channel topic
    std::set<Client*> _members;    // All clients in channel
    std::set<Client*> _operators;  // Channel operators

public:
    Channel(const std::string& name);
    ~Channel();

    // Membership
    void addClient(Client* client);
    void removeClient(Client* client);
    bool hasClient(Client* client) const;

    // Topic
    void setTopic(const std::string& topic);
    std::string getTopic() const;

    // Broadcast
    void broadcast(const std::string& msg, Client* except = NULL);
};

#endif
