#pragma once

#include <functional>
#include <string>

#include "core/ui/clickable.hpp"
#include "core/text.hpp"

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
    bool OnUpdate() override;
protected:
    void OnClick() override { m_onClick(); }
private:
    std::function<void()> m_onClick;
    std::string m_textureName;
    Text m_text;
};
