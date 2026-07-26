#pragma once
#include <optional>
#include <string>

#include "raylib.h"
#include "services.hpp"
#include "utils/util.hpp"
#include "rendering/shader.hpp"

struct SliceMargins {
    int left;
    int top;
    int right;
    int bottom;
};

struct TextureSpec {
    std::string texture;
    std::optional<SliceMargins> slice{std::nullopt};
    Color tint{WHITE};
    std::optional<ShaderEffect> processing{std::nullopt};

    void Draw(Rectangle dest) const {
        auto tex = GetServices().resManager.getTexture(texture);

        auto drawCall = [&, this]() {
            if (slice) {
                auto& s = *slice;
                NPatchInfo npi = {
                    rect(tex),
                    s.left, s.top, s.right, s.bottom,
                    NPATCH_NINE_PATCH
                };
                DrawTextureNPatch(tex, npi, dest, {0, 0}, 0, tint);
            } else {
                DrawTexturePro(tex, rect(tex), dest, {0, 0}, 0, tint);
            }
        };

        if (processing) {
            auto& p = *processing;
            useShaderUnchecked(p.programName, p.uniforms, drawCall);
        } else {
            drawCall();
        }
    }
};