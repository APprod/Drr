#include "core/debugOverlay.hpp"
#include "core/structs.hpp"
#include "core/debug.hpp"
#include <format>

DebugOverlay::DebugOverlay(UIComponentSpec uiSpec, LayoutSpec layoutSpec)
: Stack(uiSpec, layoutSpec) {
    
    interactive = false;

    auto left = std::make_unique<VerticalLayout>(
        UICSpec{}.SetPaddingPct({1,1,1,1}),
        LayoutSpec{}.AlignBegin().CrossBegin());
    left->Add(
        FPSDraw("", {}, "TNR", 32, 1),
        CursorTrack("", {}, "TNR", 32, 1),
        DebugLogDisplay("", {}, "TNR", 32, 1)
    );
    AddChild(std::move(left));

    auto right = std::make_unique<VerticalLayout>(
        UICSpec{}.SetPaddingPct({1,1,1,1}),
        LayoutSpec{}.AlignBegin().CrossEnd());
    right->Add(
        PerformanceDisplay("", {}, "TNR", 32, 1)
    );
    AddChild(std::move(right));
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

bool PerformanceDisplay::OnUpdate(){
    if (!GetServices().runtimeCfg.showPerformance) {
        SetText(""); return Label::OnUpdate();
    }
    std::string out;
    for (auto& [name, stat] : GetServices().perfLog.getLogData()) {
        out += std::format("{}: avg={:.2f}ms pk={:.2f}ms\n", name, stat.average(), stat.peak());
    }
    SetText(out);
    return Label::OnUpdate();
}

bool DebugLogDisplay::OnUpdate(){
    if (!GetServices().runtimeCfg.showDebugLog) {
        SetText(""); return Label::OnUpdate();
    }
    int count = GetServices().runtimeCfg.debugMessagesCount;
    const auto& messages = dbg::GetLogger().GetMessages();
    std::string out;
    int shown = 0;
    for (int i = static_cast<int>(messages.size()) - 1; i >= 0 && shown < count; --i) {
        if (messages[i].severity == dbg::Severity::DBGINFO) continue;
        out = std::format("{}: {}\n", dbg::ToString(messages[i].severity), messages[i].message) + out;
        ++shown;
    }
    SetText(out);
    return Label::OnUpdate();
}