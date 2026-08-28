#pragma once
#include "raylib.h"
#include "rendering/shader.hpp"
#include <vector>

// Helper class with analogue of Raylib function that implements stacking for them
class Renderer {
public:
    // calls raylib function and saves state to a stack
    void beginTextureMode(RenderTexture2D& target);
    // calls function and restores previus state if left
    void endTextureMode();

    void beginShaderMode(const std::string& name, const Uniforms& uniforms); // calls raylib function and saves state to a stack
    void beginShaderMode(const ShaderProgram& program, const Uniforms& uniforms); // calls raylib function and saves state to a stack
    void beginShaderMode(const ShaderInput& input);// calls raylib function and saves state to a stack
    // calls function and restores previus state if left
    void endShaderMode();
    
    // calls raylib function and saves state to a stack
    void beginBlendMode(BlendMode mode);
    // calls function and restores previus state if left
    void endBlendMode();
private:
    std::vector<RenderTexture2D*> m_textureStack;
    std::vector<ShaderInput> m_shaderStack;
    std::vector<BlendMode> m_blendStack;
};
