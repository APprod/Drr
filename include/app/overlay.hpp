#pragma once
#include "core/ui.hpp"

class Overlay : public Stack {
public:
    Overlay(UIComponentSpec uiSpec = {}, LayoutSpec layoutSpec = base);
};
