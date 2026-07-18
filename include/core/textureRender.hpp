#pragma once
#include "raylib.h"
#include <vector>
#include "core/log.hpp"

class Renderer {
public:
    void beginTextureMode(RenderTexture2D& target) {
        ::BeginTextureMode(target);
        m_stack.push_back(&target);
    }

    void endTextureMode() {
        if (m_stack.empty()) {
            mylog::GetLogger().Error("Rendere::endTextureMode called but beginTextureMode wasn't registered. Maybe Used Raylib global function?");
            return;
        }
        ::EndTextureMode();
        m_stack.pop_back();
        if (!m_stack.empty()) {
            ::BeginTextureMode(*m_stack.back());
        }
    }
private:
    std::vector<RenderTexture2D*> m_stack;
};