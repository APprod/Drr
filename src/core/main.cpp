#include "core/app.hpp"
#include "core/log.hpp"

#include <iostream>

#ifdef TRACY_PROFILE
#include <tracy/Tracy.hpp>
#endif

int main(void)
{
    App myapp(800, 450);
    myapp.init();
    myapp.run();
    myapp.close();

    std::cerr << "finished" << std::endl;
    return 0;
}