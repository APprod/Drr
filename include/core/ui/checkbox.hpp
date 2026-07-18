#pragma once

#include <functional>

#include "core/ui/layout.hpp"
#include "core/rendering/text.hpp"

class Checkbox : public HorizontalLayout {
public:
    Checkbox(
        Text text,
        bool* value,
        std::function<void(bool)> onChange = nullptr,
        UIComponentSpec spec = {}
    );
};
