
#include "../includes/DCCTransfer.hpp"
#include <cstdlib>
#include <sstream>

DCCTransfer::DCCTransfer(const std::string& targetNick,
                         const std::string& filename,
                         const std::string& filesizeStr)
    : _filename(filename), _targetNick(targetNick), _filesizeStr(filesizeStr)
{}

void DCCTransfer::start()
{
    std::cout << "\033[32mStarting file transfer to " << _targetNick << "...\033[0m\n";
    std::cout << "\033[33m📁 Incoming file for " << _targetNick
              << ": " << _filename
              << " (" << _filesizeStr << " bytes)\033[0m\n";

    std::ifstream infile(_filename.c_str(), std::ios::binary);
    if (!infile)
    {
        std::cerr << "\033[31mERROR: Cannot open source file: " << _filename << "\033[0m\n";
        return;
    }

    const char* home = std::getenv("HOME");
    std::string destPath = home ? std::string(home) + "/_copy_" + _filename : "_copy_" + _filename;

    std::ofstream outfile(destPath.c_str(), std::ios::binary);
    if (!outfile)
    {
        std::cerr << "\033[31mERROR: Cannot create destination file: " << destPath << "\033[0m\n";
        return;
    }

    char buffer[4096];
    size_t totalCopied = 0;
   std::stringstream ss(_filesizeStr);
   size_t filesizeInt = 0;
    ss >> filesizeInt;

while (infile.good())
    {
        infile.read(buffer, sizeof(buffer));
        std::streamsize bytes = infile.gcount();
        if (bytes > 0)
        {
            outfile.write(buffer, bytes);
            totalCopied += bytes;
            std::cout << "\033[32mFile transfer complete! (" 
                    << totalCopied << " bytes received)\033[0m\n";
        }
    }
}
