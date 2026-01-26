#pragma once

#include <string>
#include <iostream>
#include <cassert>

#ifdef M_Deb
#define M_ASSERT( expected,  message, critic, quiet ) \
       if (!(expected))\
        { \
            if (!(quiet))\
            {\
                std::cerr << "Asertion failed"    << "\n";\
                std::cerr << "---------------"    << "\n";\
                std::cerr << message              << "\n";\
                std::cerr << "---------------"    << "\n";\
                std::cerr << __FILE__             << "\n";\
                std::cerr << "Line:" << __LINE__  << "\n";\
                std::cerr << "---------------"    << "\n";\
            } else{\
                std::cerr << message              << "\n";\
            }\
            if (critic == fwk::A_L::high)\
            {\
                std::abort();\
            }\
            if (critic == fwk::A_L::mid)\
            {\
                if (fwk::ex_dialog()) exit(-1);\
               \
            }\
        }
#else
#define M_ASSERT( expected,  message, critic, quiet ) 
#endif

namespace fwk
{
    void D_Pr(std::string message, bool Dk = true);

    
    template<class T>
    void D_Pr(std::string message, T tval, bool Dk = true)
    {
        if (!Dk) return;
        std::cerr << message << tval << std::endl;
    }
    
    enum class A_L
    {
        low = 0,
        mid = 1,
        high = 2
    };

    //inline void M_ASSERT(bool expected, std::string message, A_L critic = A_L::low, bool quiet = false);
    bool ex_dialog();

    
}