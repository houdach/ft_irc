#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
class Client;

class Channel
{
    private:
        std::string name;
        std::string topic;
        std::string key;
        bool inviteOnly;
        bool topicRestricted;
        int userLimit;
        std::vector<Client*> users;
        std::vector<Client*> operators;
        std::vector<std::string> invited;

    public:
        Channel();
        Channel(const std::string& name);
        ~Channel();

        void addUser(Client* client);
        void removeUser(Client* client);
        void addOperator(Client* client);
        void removeOperator(Client* client);

        bool hasUser(Client* client) const;
        bool isOperator(Client* client) const;

        void broadcast(const std::string& message, Client* sender = NULL);

        std::string getName() const;
        std::string getTopic() const;
        std::string getKey() const;
        bool getInviteOnly() const;
        bool getTopicRestricted() const;
        int getUserLimit() const;
        std::vector<Client*> getUsers() const;
        std::vector<Client*> getOperators() const;
        size_t getUserCount() const;

        void setTopic(const std::string& newTopic);
        void setKey(const std::string& newKey);
        void setInviteOnly(bool invite);
        void setTopicRestricted(bool restricted);
        void setUserLimit(int limit);

        // Invite list handling
        void addInvite(const std::string& nick);
        bool isInvited(const std::string& nick) const;
        void removeInvite(const std::string& nick);

        bool isFull() const;
};

#endif
