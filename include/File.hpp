#ifndef FILE_HPP
#define FILE_HPP

#include <string>

class File {
private:
    std::string _filename;   // Name of the file
    std::string _sender;     // Nick of sender
    std::string _receiver;   // Nick of receiver
    size_t _size;            // File size in bytes
    bool _active;            // Transfer in progress?

public:
    File(const std::string& filename, const std::string& sender, const std::string& receiver, size_t size);
    ~File();

    void start();
    void cancel();
    bool isActive() const;

    std::string getFilename() const;
    std::string getSender() const;
    std::string getReceiver() const;
    size_t getSize() const;
};

#endif
