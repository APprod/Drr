#include "app.hpp"
#include "utils/log.hpp"

#include <iostream>

#ifdef TRACY_PROFILE
#include <tracy/Tracy.hpp>
#endif

int main(void)
{
    auto app = createApp();
    Engine engine{std::move(app)};

    engine.init();
    engine.run();
    engine.close();

    std::cerr << "finished" << std::endl;
    return 0;
}