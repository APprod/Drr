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

//uses shader, checks for typos in uniforms names
void useShader(const ShaderProgram& program, const Uniforms& uniforms, std::function<void()> drawCall);

//uses shader, checks for typos in uniforms names
void useShader(std::string name, const Uniforms& uniforms, std::function<void()> drawCall);

//uses shader without checking for typos in uniforms names
void useShaderUnchecked(const ShaderProgram& program, const Uniforms& uniforms, std::function<void()> drawCall);

//uses shader without checking for typos in uniforms names
void useShaderUnchecked(std::string name, const Uniforms& uniforms, std::function<void()> drawCall);

void setShaderUnchecked(const ShaderProgram& program, const Uniforms& uniforms);