/*just definitions of all structs used in different parts*/
#pragma once
#include "raylib.h"
#include <string>
#include <cstdint>
#include "core/recManager.hpp"
#include "core/myinput.hpp"
#include "core/runtimeConfig.hpp"


struct Services
{
    float dt = 16.7f;
    RecourceManager recManager;
    MyInput input;
    PerformanceLog perfLog{20};
    RuntimeConfig runtimeCfg;

    static Services& Get(){
        static Services services;
        return services;
    }
private:
    Services() = default;
    Services(const Services&) = delete;
    Services& operator=(const Services&) = delete;
};

inline Services& GetServices() {
    return Services::Get();
}
