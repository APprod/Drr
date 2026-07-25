#pragma once
#include "rendering/resManager.hpp"
#include "input/myinput.hpp"
#include "runtimeConfig.hpp"
#include "scene/sceneManager.hpp"
#include "textureRender.hpp"

struct Services
{
    ResourceManager resManager;
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
