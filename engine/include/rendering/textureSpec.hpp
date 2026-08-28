#pragma once
#include <optional>
#include <string>

#include <nlohmann/json.hpp>

#include "raylib.h"
#include "rendering/shader.hpp"
// Data for 9-Patch texture rendering
struct SliceMargins {
    int left;
    int top;
    int right;
    int bottom;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SliceMargins, left, top, right, bottom)
};
// Class defines a texture that is drawn 
// Adds helpers to simplify the use
class TextureSpec {
public:
    TextureSpec() = default;

    explicit TextureSpec(const std::string& textureName);

    TextureSpec& Shader(ShaderEffect e) { processing = std::move(e); return *this; }
    TextureSpec& Slice(SliceMargins s) { slice = std::move(s); return *this; }

    void Draw(Rectangle dest) const;

    std::string texture;
    std::optional<SliceMargins> slice;
    Color tint{WHITE};
    std::optional<ShaderEffect> processing;
};