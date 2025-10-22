#include "../include/File.hpp"

File::File(const std::string& filename, const std::string& sender, 
           const std::string& receiver, size_t size)
    : _filename(filename), _sender(sender), _receiver(receiver), 
      _size(size), _active(false)
{
}

File::~File() 
{
}

void File::start() 
{
    _active = true;
}

void File::cancel() 
{
    _active = false;
}

bool File::isActive() const 
{
    return _active;
}

std::string File::getFilename() const 
{
    return _filename;
}

std::string File::getSender() const 
{
    return _sender;
}

std::string File::getReceiver() const 
{
    return _receiver;
}

size_t File::getSize() const 
{
    return _size;
}
