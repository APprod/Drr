#pragma once

#include <format>
#include <string>
#include <type_traits>

#include "core/ui/label.hpp"

template<typename T>
requires std::is_arithmetic_v<T>
class ValueLabel : public Label {
public:
    ValueLabel(std::string fmt, T* value, Text text, UIComponentSpec spec = {})
        : Label(std::move(text), spec)
        , m_fmt(std::move(fmt))
        , m_value(value)
    {}

    bool OnUpdate() override {
        if (m_value) {
            SetText(std::vformat(m_fmt, std::make_format_args(*m_value)));
        }
        return Label::OnUpdate();
    }

private:
    std::string m_fmt;
    T* m_value;
};
