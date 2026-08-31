#pragma once

#include <format>
#include <string>
#include <type_traits>

#include "ui/label.hpp"

// Displays value of variable with provided formatting
template<typename T>
requires std::is_arithmetic_v<T>
class ValueLabel : public Label {
public:
    ValueLabel(std::string fmt, T* value, Text text, UIComponentSpec spec = {})
        : Label(std::move(text), spec)
        , m_fmt(std::move(fmt))
        , m_value(value)
    {}

    bool OnUpdate(float dt) override {
        if (m_value) {
            SetText(std::vformat(m_fmt, std::make_format_args(*m_value)));
        }
        return Label::OnUpdate(dt);
    }

private:
    std::string m_fmt;
    T* m_value;
};
