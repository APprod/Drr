#include "debug.hpp"

#include <string>
#include <iostream>
#include <cassert>

namespace fwk
{
    
    bool ex_dialog()
    {
        std::cerr << "continue? 1/0" << std::endl; 
        int res;
        std::cin >> res;
        M_ASSERT(res == 1 || res == 0, "input 1/0", fwk::A_L::low, true);
        return res;
    }

    void D_Pr(std::string message, bool Dk)
    {
        if (!Dk) return;
        std::cerr << message << "\n";
    }

    
}
