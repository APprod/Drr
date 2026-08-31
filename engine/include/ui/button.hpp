#pragma once

#include <functional>

#include "ui/clickable.hpp"
#include "rendering/text.hpp"
#include "utils/animated.hpp"

// Basic button implementation, has animation 
class Button: public Clickable{
public:
    Button(
        Text text,
        std::function<void()> onClick,
        TextureSpec textureSpec,
        Vector2 targetSize,
        UIComponentSpec spec = {}
    );
    void MeasureContent(Vector2 available) override;
    bool OnUpdate(float dt) override;
protected:
    Padding ResolvePadding(Vector2 dims) const override;
    void OnDrawContent() override;
    void OnClick() override { m_onClick(); }
    std::function<void()> m_onClick;
    TextureSpec m_textureSpec;
    Text m_text;
    Animated<float> m_brightness{1.0f, 0.12f, Easing::easeOutCubic};
    Animated<float> m_scale{1.0f, 0.10f, Easing::easeOutBack};
};
