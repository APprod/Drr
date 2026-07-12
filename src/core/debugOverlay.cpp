#include "core/debugOverlay.hpp"
#include "core/structs.hpp"
#include <format>

DebugOverlay::DebugOverlay(UIComponentSpec uiSpec, LayoutSpec layoutSpec)
: Stack(uiSpec, layoutSpec) {
    
    interactive = false;
    auto column = std::make_unique<VerticalLayout>(
        UICSpec{}.SetPaddingPct({1,1,1,1}),
        LayoutSpec{}.AlignBegin().CrossBegin());
    column->Add(
        FPSDraw("", {}, "TNR", 32, 2),
        CursorTrack("", {}, "TNR", 32, 2)
    );
    AddChild(std::move(column));
}


bool FPSDraw::OnUpdate(){
    if (GetServices().runtimeCfg.showFPS)
        SetText(std::to_string(GetFPS()) + " FPS");
    else
        SetText("");
    return Label::OnUpdate();
}

EventResult CursorTrack::OnEvent(const MyEvent& event){
    if (std::holds_alternative<CursorMoveEvent>(event)){
        const auto& pos = std::get<CursorMoveEvent>(event).pos;
        m_pos = pos;
    }
    return EventResult::NotHandled;
}


bool CursorTrack::OnUpdate(){
    if (GetServices().runtimeCfg.showCursorPos)
        SetText(std::format("Cursor pos: {:04d} {:04d}",
            static_cast<int>(m_pos.x), static_cast<int>(m_pos.y)));
    else
        SetText("");
    return Label::OnUpdate();
}