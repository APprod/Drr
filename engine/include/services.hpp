#pragma once
#include "rendering/resManager.hpp"
#include "input/myinput.hpp"
#include "runtimeConfig.hpp"
#include "scene/sceneManager.hpp"
#include "rendering/renderer.hpp"
#include "utils/loader.hpp"
#include "utils/perf.hpp"
#include "ui/uiTheme.hpp"
#include "userSettings.hpp"

void setupDebugConfig();

//Service locator singleton
struct Services
{
    // Helper services
    ResourceManager resManager;
    UITheme theme;
    MyInput input;
    PerformanceLog perfLog{60};
    SceneManager sceneManager;
    Renderer renderer;
    loader::Loader loader;
    // Data
    DebugFlags debugFlags;
    UserSettings userSettings;

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
