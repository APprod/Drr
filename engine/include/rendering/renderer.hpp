#pragma once
#include "raylib.h"
#include "rendering/shader.hpp"
#include <vector>


class Renderer {
public:
    void beginTextureMode(RenderTexture2D& target);
    void endTextureMode();

    void beginShaderMode(const std::string& name, const Uniforms& uniforms);
    void beginShaderMode(const ShaderProgram& program, const Uniforms& uniforms);
    void endShaderMode();
    void beginBlendMode(BlendMode mode);
    void endBlendMode();
private:
    std::vector<RenderTexture2D*> m_textureStack;
    std::vector<ShaderInput> m_shaderStack;
    std::vector<BlendMode> m_blendStack;
};
