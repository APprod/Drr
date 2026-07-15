#include "core/shader.hpp"
#include "core/debug.hpp"

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

void useShader(const ShaderProgram& program, const Uniforms& uniforms, std::function<void()> drawCall){
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
