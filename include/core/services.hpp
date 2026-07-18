#pragma once
#include "core/resManager.hpp"
#include "core/myinput.hpp"
#include "core/runtimeConfig.hpp"
#include "core/sceneManager.hpp"
#include "core/textureRender.hpp"

struct Services
{
    ResourceManager recManager;
    MyInput input;
    PerformanceLog perfLog{60};
    RuntimeConfig runtimeCfg;
    SceneManager sceneManager;
    Renderer renderer;

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
