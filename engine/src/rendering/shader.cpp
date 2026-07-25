#include "rendering/shader.hpp"
#include "utils/log.hpp"
#include "services.hpp"

namespace {

void SetUniform(const Shader& shader, int location, const UniformValue& value) {
    std::visit([&](auto&& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, float>)
            ::SetShaderValue(shader, location, &v, SHADER_UNIFORM_FLOAT);
        else if constexpr (std::is_same_v<T, int>)
            ::SetShaderValue(shader, location, &v, SHADER_UNIFORM_INT);
        else if constexpr (std::is_same_v<T, Vector2>)
            ::SetShaderValue(shader, location, &v, SHADER_UNIFORM_VEC2);
        else if constexpr (std::is_same_v<T, Vector3>)
            ::SetShaderValue(shader, location, &v, SHADER_UNIFORM_VEC3);
        else if constexpr (std::is_same_v<T, Vector4>)
            ::SetShaderValue(shader, location, &v, SHADER_UNIFORM_VEC4);
    }, value);
}

}

void useShaderUnchecked(const ShaderProgram& program, const Uniforms& uniforms, std::function<void()> drawCall){
    auto& shader = program.shader;
    auto& locs = program.locations;

    ::BeginShaderMode(shader);

    for (auto& [name, loc]: locs){
        auto it = uniforms.find(name);
        if (it != uniforms.end()){
            SetUniform(shader, loc, it->second);
        } else{
            SetUniform(shader, loc, program.defaults.at(name));
        }
    }

    drawCall();
    ::EndShaderMode();
}

void useShader(const ShaderProgram& program, const Uniforms& uniforms, std::function<void()> drawCall){
    auto& shader = program.shader;
    auto& locs = program.locations;

    for (const auto& [name, _] : uniforms) {
        if (!locs.contains(name)) {
            auto unisNames = getKeys(locs);
            std::string all = "";
            for (auto& thisName: unisNames)
                all += ", " + thisName;
            mylog::GetLogger().Error(std::format("INCORRECT UNIFORM LOCATION NAME, CHECK FOR TYPOS. Uniform name: {}, available:{}", name, all));
        }
    }

    ::BeginShaderMode(shader);

    for (auto& [name, loc]: locs){
        auto it = uniforms.find(name);
        if (it != uniforms.end()){
            SetUniform(shader, loc, it->second);
        } else{
            SetUniform(shader, loc, program.defaults.at(name));
        }
    }

    drawCall();
    ::EndShaderMode();
}




void useShaderUnchecked(std::string name, const Uniforms& uniforms, std::function<void()> drawCall){
    auto shaderProgram = GetServices().resManager.getShaderProgram(name);
    useShaderUnchecked(shaderProgram, uniforms,drawCall);
}


void useShader(std::string name, const Uniforms& uniforms, std::function<void()> drawCall){
    auto shaderProgram = GetServices().resManager.getShaderProgram(name);
    useShader(shaderProgram, uniforms,drawCall);
}