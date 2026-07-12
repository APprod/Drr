#include "core/debugOverlay.hpp"
#include "core/structs.hpp"

DebugOverlay::DebugOverlay(UIComponentSpec uiSpec, LayoutSpec layoutSpec)
: Stack(uiSpec, layoutSpec) {
    
    interactive = false;
    auto column = std::make_unique<VerticalLayout>(
        UICSpec{}.SetPaddingPct({1,1,1,1}),
        LayoutSpec{}.AlignBegin().CrossBegin());
    column->Add(
        FPSDraw("",{10,10},{}),
        CursorTrack("", {10,10})
    );
    AddChild(std::move(column));
}


void FPSDraw::OnUpdate(){
    if (GetServices().runtimeCfg.showFPS){
        m_text = std::to_string(GetFPS()) + " FPS";
    }else{
        m_text = "";
    }
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
    if (GetServices().runtimeCfg.showCursorPos){
        m_text = toString(m_pos);
    }else{
        m_text = "";
    }
    Label::OnUpdate();
}