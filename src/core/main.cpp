#include "core/app.hpp"
#include "core/debug.hpp"

#include <iostream>

int main(void)
{
    App myapp(800, 450);
    myapp.init();
    myapp.run();
    myapp.close();

    std::cerr << "finished" << std::endl;
    return 0;
}