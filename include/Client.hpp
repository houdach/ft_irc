#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

class Client {
private:
    int _fd;               
    std::string _nick;    
    std::string _username;    
    std::string _realname;    
    bool _registered;        
    bool _passwordAuthenticated; 

    std::string _bufferIn;   
    std::vector<std::string> _bufferOut; 

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
    std::string popLine();  
    void queueMessage(const std::string& msg);
    std::string nextMessage();
};

#endif
