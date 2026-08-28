#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <functional>
#include "raylib.h"


using UniformValue = std::variant<float, int, Vector2, Vector3, Vector4>;
using Uniforms = std::unordered_map<std::string, UniformValue>;

// Main persistent description of shader
struct ShaderProgram{
    Shader shader;
    std::unordered_map<std::string, int> locations;
    Uniforms defaults;
};
// Description of shader for shader draw call, has actual uniforms.
struct ShaderInput{
    ShaderProgram program;
    Uniforms uniforms;
};

struct ShaderEffect {
    std::string programName = "processing";
    Uniforms uniforms;
};

/* Premade shader helpers */
class ProcessingSh {
public:
    ProcessingSh() = default;
    ProcessingSh& saturation(float v)  { m_uniforms["saturation"] = v; return *this; }
    ProcessingSh& brightness(float v)  { m_uniforms["brightness"] = v; return *this; }
    ProcessingSh& contrast(float v)    { m_uniforms["contrast"] = v; return *this; }
    ProcessingSh& gamma(float v)       { m_uniforms["gamma"] = v; return *this; }
    ProcessingSh& tint(Vector3 v)      { m_uniforms["tint"] = v; return *this; }
    ProcessingSh& alpha(float v)       { m_uniforms["alpha"] = v; return *this; }

    operator ShaderEffect() const { return {"processing", m_uniforms}; }
private:
    Uniforms m_uniforms;
};

class BrightnessSh {
public:
    explicit BrightnessSh(float v = 1.0f) { Set(v); }
    BrightnessSh& Set(float v) { m_uniforms["brightness"] = v; return *this; }

    operator ShaderEffect() const { return {"brightness", m_uniforms}; }
private:
    Uniforms m_uniforms;
};


//uses shader, checks for typos in uniforms names
void useShader(const ShaderProgram& program, const Uniforms& uniforms, std::function<void()> drawCall);

//uses shader, checks for typos in uniforms names
void useShader(std::string name, const Uniforms& uniforms, std::function<void()> drawCall);

//uses shader without checking for typos in uniforms names
void useShaderUnchecked(const ShaderProgram& program, const Uniforms& uniforms, std::function<void()> drawCall);

//uses shader without checking for typos in uniforms names
void useShaderUnchecked(std::string name, const Uniforms& uniforms, std::function<void()> drawCall);
// Starts shader mode + sets uniforms without shades stack from renderer
// Breaks shader stacking
void setShaderUnchecked(const ShaderProgram& program, const Uniforms& uniforms);