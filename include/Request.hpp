#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>

class Request {
private:
    std::string _prefix;           // Optional :prefix
    std::string _command;          // Command name (NICK, USER, JOIN…)
    std::vector<std::string> _params; // Parameters

public:
    Request();
    ~Request();

    static Request parse(const std::string& line);

    std::string getCommand() const;
    const std::vector<std::string>& getParams() const;
    std::string getPrefix() const;
};

#endif
