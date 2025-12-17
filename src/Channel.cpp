/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zchtaibi <zchtaibi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 17:24:32 by zchtaibi          #+#    #+#             */
/*   Updated: 2025/10/06 17:08:20 by zchtaibi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include <algorithm>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

Channel::Channel() 
    : name(""), topic(""), key(""), inviteOnly(false), 
      topicRestricted(true), userLimit(-1) 
{
}

Channel::Channel(const std::string& channelName) 
    : name(channelName), topic(""), key(""), inviteOnly(false), 
      topicRestricted(true), userLimit(-1) 
{
}

Channel::~Channel() 
{
}

void Channel::addUser(Client* client) 
{
    if (!hasUser(client))
        users.push_back(client);
}

void Channel::removeUser(Client* client) 
{
    std::vector<Client*>::iterator it = std::find(users.begin(), users.end(), client);
    if (it != users.end())
        users.erase(it);
    
    // Also remove from operators if present
    it = std::find(operators.begin(), operators.end(), client);
    if (it != operators.end())
        operators.erase(it);
}

    void Channel::addInvite(const std::string& nick)
    {
        // avoid duplicates
        for (size_t i = 0; i < invited.size(); ++i)
            if (invited[i] == nick)
                return;
        invited.push_back(nick);
    }

    bool Channel::isInvited(const std::string& nick) const
    {
        for (size_t i = 0; i < invited.size(); ++i)
            if (invited[i] == nick)
                return true;
        return false;
    }

    void Channel::removeInvite(const std::string& nick)
    {
        std::vector<std::string>::iterator it = std::find(invited.begin(), invited.end(), nick);
        if (it != invited.end())
            invited.erase(it);
    }

void Channel::addOperator(Client* client) 
{
    if (!isOperator(client))
        operators.push_back(client);
}

void Channel::removeOperator(Client* client) 
{
    std::vector<Client*>::iterator it = std::find(operators.begin(), operators.end(), client);
    if (it != operators.end())
        operators.erase(it);
}

bool Channel::hasUser(Client* client) const 
{
    return std::find(users.begin(), users.end(), client) != users.end();
}

bool Channel::isOperator(Client* client) const 
{
    return std::find(operators.begin(), operators.end(), client) != operators.end();
}

void Channel::broadcast(const std::string& message, Client* sender) 
{
    for (size_t i = 0; i < users.size(); i++) 
    {
        if (users[i] != sender) 
        {
            std::string msg = message;
            if (msg.find("\r\n") == std::string::npos)
                msg += "\r\n";
            send(users[i]->getFd(), msg.c_str(), msg.length(), 0);
        }
    }
}

std::string Channel::getName() const 
{
    return name;
}

std::string Channel::getTopic() const 
{
    return topic;
}

std::string Channel::getKey() const 
{
    return key;
}

bool Channel::getInviteOnly() const 
{
    return inviteOnly;
}

bool Channel::getTopicRestricted() const 
{
    return topicRestricted;
}

int Channel::getUserLimit() const 
{
    return userLimit;
}

std::vector<Client*> Channel::getUsers() const 
{
    return users;
}

std::vector<Client*> Channel::getOperators() const 
{
    return operators;
}

size_t Channel::getUserCount() const 
{
    return users.size();
}

void Channel::setTopic(const std::string& newTopic) 
{
    topic = newTopic;
}

void Channel::setKey(const std::string& newKey) 
{
    key = newKey;
}

void Channel::setInviteOnly(bool invite) 
{
    inviteOnly = invite;
}

void Channel::setTopicRestricted(bool restricted) 
{
    topicRestricted = restricted;
}

void Channel::setUserLimit(int limit) 
{
    userLimit = limit;
}

bool Channel::isFull() const 
{
    if (userLimit <= 0)
        return false;
    return static_cast<int>(users.size()) >= userLimit;
}
