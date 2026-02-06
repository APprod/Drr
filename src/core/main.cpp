#define M_Deb true

#include "core/app.hpp"
#include "core/debug.hpp"

#include <iostream>

// #include "test.hpp"



int main(void)
{
    App myapp(800, 450);
    myapp.run();
    
    std::cerr << "finished" << std::endl;
    return 0;
}