#pragma once
#include "core/ui/layout.hpp"

class Overlay : public Stack {
public:
    Overlay(UIComponentSpec uiSpec = {}, LayoutSpec layoutSpec = base);
};
