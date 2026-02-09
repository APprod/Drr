#pragma once
#include "core/ui.hpp"

class TestComp: public UIComponent{
public:
    TestComp(Vector2 targetSize);
    void OnUpdate() override;
    void OnDraw() override;
    void MeasureContent(Vector2 available) override;
    void ArrangeContent(Rectangle actualRect) override;
};
