#include "rendering/renderer.hpp"
#include "utils/log.hpp"
#include "services.hpp"

void Renderer::beginTextureMode(RenderTexture2D& target) {
    ::BeginTextureMode(target);
    m_textureStack.push_back(&target);
}

void Renderer::endTextureMode() {
    if (m_textureStack.empty()) {
        mylog::GetLogger().Error("Renderer::endTextureMode called but beginTextureMode wasn't registered. Maybe Used Raylib global function?");
        return;
    }
    ::EndTextureMode();
    m_textureStack.pop_back();
    if (!m_textureStack.empty()) {
        ::BeginTextureMode(*m_textureStack.back());
    }
}

void Renderer::beginShaderMode(const ShaderProgram& program, const Uniforms& uniforms){
    setShaderUnchecked(program, uniforms);
    m_shaderStack.push_back({program, uniforms});
}

void Renderer::beginShaderMode(const std::string& name, const Uniforms& uniforms){
    beginShaderMode(GetServices().resManager.getShaderProgram(name), uniforms);
}

void Renderer::endShaderMode(){
    ::EndShaderMode();
    if (m_shaderStack.empty()) {
        mylog::GetLogger().Error("Renderer::endShaderMode called but beginShaderMode wasn't registered. Maybe Used Raylib global function?");
        return;
    }
    m_shaderStack.pop_back();
    if (!m_shaderStack.empty()) {
        auto& [program, uniforms] = m_shaderStack.back();
        setShaderUnchecked(program, uniforms);
    }
}

void Renderer::beginBlendMode(BlendMode mode){
    ::BeginBlendMode(mode);
    m_blendStack.push_back(mode);
}

void Renderer::endBlendMode(){
    ::EndBlendMode();
    m_blendStack.pop_back();
    if (!m_blendStack.empty()) {
        ::BeginBlendMode(m_blendStack.back());
    }
}   