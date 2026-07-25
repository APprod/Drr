#pragma once

#include <functional>

#include "ui/layout.hpp"
#include "rendering/text.hpp"

class Checkbox : public HorizontalLayout {
public:
    Checkbox(
        Text text,
        bool* value,
        std::function<void(bool)> onChange = nullptr,
        UIComponentSpec spec = {}
    );
};
