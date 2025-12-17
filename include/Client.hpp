#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

class Client {
private:
    int _fd;                  // Socket FD
    std::string _nick;        // Nickname
    std::string _username;    // Username
    std::string _realname;    // Real name
    bool _registered;         // Has finished PASS+NICK+USER?
    bool _passwordAuthenticated; // Has provided correct password

    std::string _bufferIn;    // Data received but not yet parsed
    std::vector<std::string> _bufferOut; // Messages to send

public:
    Client(int fd);
    ~Client();

    // Basic info
    int getFd() const;
    std::string getNick() const;
    void setNick(const std::string& nick);
    std::string getUsername() const;
    void setUsername(const std::string& username);
    std::string getRealname() const;
    void setRealname(const std::string& realname);

    bool isRegistered() const;
    void setRegistered(bool status);

    bool isPasswordAuthenticated() const;
    void setPasswordAuthenticated(bool status);

    // Communication
    void appendIncoming(const std::string& data);
    std::string popLine();   // Get one IRC line (\r\n terminated)
    void queueMessage(const std::string& msg);
    std::string nextMessage();
};

#endif
