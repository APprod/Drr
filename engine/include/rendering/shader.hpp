#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <functional>
#include "raylib.h"

using UniformValue = std::variant<float, int, Vector2, Vector3, Vector4>;

using Uniforms = std::unordered_map<std::string, UniformValue>;

struct ShaderProgram{
    Shader shader;
    std::unordered_map<std::string, int> locations;
    Uniforms defaults;
};

struct ShaderInput{
    ShaderProgram program;
    Uniforms uniforms;
};


void useShader(const ShaderProgram& program, const Uniforms& uniforms, std::function<void()> drawCall);
void useShader(std::string name, const Uniforms& uniforms, std::function<void()> drawCall);
void useShaderUnchecked(const ShaderProgram& program, const Uniforms& uniforms, std::function<void()> drawCall);
void useShaderUnchecked(std::string name, const Uniforms& uniforms, std::function<void()> drawCall);
void setShaderUnchecked(const ShaderProgram& program, const Uniforms& uniforms);