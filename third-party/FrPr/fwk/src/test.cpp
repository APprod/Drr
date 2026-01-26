#include <string>
#include <iostream>

namespace fwk
{
    std::string getInitMessage()
    {
        return "Successfully started fwk";
    }

    void init()
    {
        std::cerr << getInitMessage() << std::endl; 
    }
}