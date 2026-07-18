#pragma once
#include "core/rendering/resManager.hpp"
#include "core/input/myinput.hpp"
#include "core/runtimeConfig.hpp"
#include "core/scene/sceneManager.hpp"
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
