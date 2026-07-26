#pragma once

#include <functional>

#include "ui/clickable.hpp"
#include "rendering/text.hpp"
#include "rendering/textureSpec.hpp"
#include "utils/animated.hpp"

class Button: public Clickable{
public:
    Button(
        Text text,
        std::function<void()> onClick,
        TextureSpec textureSpec,
        Vector2 targetSize,
        UIComponentSpec spec = {}
    );
    void OnDrawContent() override;
    bool OnUpdate(float dt) override;
    void MeasureContent(Vector2 available) override;
protected:
    void OnClick() override { m_onClick(); }
    std::function<void()> m_onClick;
    TextureSpec m_textureSpec;
    Text m_text;
    Animated<float> m_brightness{1.0f, 0.12f, Easing::easeOutCubic};
    Animated<float> m_scale{1.0f, 0.10f, Easing::easeOutBack};
};
