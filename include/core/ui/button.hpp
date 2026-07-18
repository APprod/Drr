#pragma once

#include <functional>
#include <string>

#include "core/ui/clickable.hpp"
#include "core/text.hpp"
#include "core/animated.hpp"

class Button: public Clickable{
public:
    Button(
        Text text,
        std::function<void()> onClick,
        std::string textureName,
        Vector2 targetSize,
        UIComponentSpec spec = {}
    );
    void OnDrawContent() override;
    bool OnUpdate(float dt) override;
    void MeasureContent(Vector2 available) override;
protected:
    void OnClick() override { m_onClick(); }
    std::function<void()> m_onClick;
    std::string m_textureName;
    Text m_text;
    Animated<float> m_brightness{1.0f, 0.12f, Easing::easeOutCubic};
    Animated<float> m_scale{1.0f, 0.10f, Easing::easeOutBack};
};
