#pragma once
#include "core/ui.hpp"

class TestComp: public UIComponent{
public:
    TestComp(Vector2 targetSize);
    void OnUpdate() override;
    void OnDraw() override;
    void OnMeasure(Vector2 available) override;
    void OnArrange(Rectangle actualRect) override;
};
