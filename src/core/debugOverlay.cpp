#include "core/debugOverlay.hpp"
#include "core/structs.hpp"

DebugOverlay::DebugOverlay(UIComponentSpec uiSpec, LayoutSpec layoutSpec)
: Stack(uiSpec, layoutSpec) {
    
    interactive = false;
    auto column = std::make_unique<VerticalLayout>(
        UICSpec{}.SetPaddingPct({1,1,1,1}),
        LayoutSpec{}.AlignBegin().CrossBegin());
    column->Add(
        FPSDraw(""),
        CursorTrack("")
    );
    AddChild(std::move(column));
}


void FPSDraw::OnUpdate(){
    if (GetServices().runtimeCfg.showFPS)
        SetText(std::to_string(GetFPS()) + " FPS");
    else
        SetText("");
    Label::OnUpdate();
}

EventResult CursorTrack::OnEvent(const MyEvent& event){
    if (std::holds_alternative<CursorMoveEvent>(event)){
        const auto& pos = std::get<CursorMoveEvent>(event).pos;
        m_pos = pos;
    }
    return EventResult::NotHandled;
}


void CursorTrack::OnUpdate(){
    if (GetServices().runtimeCfg.showCursorPos)
        SetText(toString(m_pos));
    else
        SetText("");
    Label::OnUpdate();
}