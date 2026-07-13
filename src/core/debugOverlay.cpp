#include "core/debugOverlay.hpp"
#include "core/structs.hpp"
#include "core/debug.hpp"
#include <format>

DebugOverlay::DebugOverlay(UIComponentSpec uiSpec, LayoutSpec layoutSpec)
: Stack(uiSpec, layoutSpec) {
    
    interactive = true;
    int fontSize = 20;
    
    PaddingPct padBase{1,1,1,1};

    auto temp = std::make_unique<VerticalLayout>(
        UICSpec{}.SetPaddingPct(padBase),
        LayoutSpec{}.AlignBegin()
    );
    auto mainC = std::make_unique<HorizontalLayout>(
        UICSpec{}.SetPaddingPct(padBase).SetFlex({1,1}).FillMaxSize(),
        LayoutSpec{}.AlignBegin().CrossBegin().CrossShrink(true)
    );
    auto left = std::make_unique<DebugVerticalLayout>(
        UICSpec{}.SetPaddingPct(padBase).SetFlex({1,1}),
        LayoutSpec{}.AlignBegin().CrossBegin()
    );
    left->Add(
        FPSDraw(Text("", "TNR", fontSize, 0)),
        CursorTrack(Text("", "TNR", fontSize, 0)),
        DebugLogDisplay(Text("", "TNR", fontSize, 0),UICSpec{}.SetFlex({0,1}))
    );
    auto right = std::make_unique<DebugVerticalLayout>(
        UICSpec{}.SetPaddingPct(padBase).SetFlex({1,1}).FillMaxHeight(),
        LayoutSpec{}.AlignBegin().CrossEnd());
    right->Add(
            PerformanceDisplay(Text("", "TNR", fontSize, 0),UICSpec{}.SetFlex({0,1})),
            Label(Text("F1 - overlay\nF2 - layout\nF3 - content\nF4 - fps\nF5 - cursor\nF6 - perf\nF7 - log", "TNR", fontSize, 0))
    );
    mainC->AddChild(std::move(left));
    mainC->AddChild(std::move(right));

    auto tempBottom = std::make_unique<HorizontalLayout>(
        UICSpec{}.SetPaddingPct(padBase).SetFlex({1,1}).FillMaxSize(),
        LayoutSpec{}.AlignBegin().CrossBegin()
    );
    temp->AddChild(std::move(mainC));
    temp->AddChild(std::move(tempBottom));
    AddChild(std::move(temp));
}

bool DebugOverlay::OnUpdate(){
    visible = GetServices().runtimeCfg.showDebugOverlay;
    interactive = visible;
    return Stack::OnUpdate();
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


void DebugVerticalLayout::OnDrawContent(){
    auto r = actual;
    ::BeginBlendMode(BlendMode::BLEND_MULTIPLIED);
    ::DrawRectangleGradientV(r.x,r.y,r.width,r.height, {0,0,50,255}, {0,0,0,0});
    ::EndBlendMode();
    for(auto& child : children){
        if (!child->visible) continue;
        child->OnDraw();
    }
}
void DebugHorizontalLayout::OnDrawContent(){
    auto r = GetDrawRect();
    // ::BeginBlendMode(BlendMode::BLEND_ADD_COLORS);
    ::DrawRectangleGradientV(r.x,r.y,r.width,r.height, {0,255,0,255}, {0,0,255,0});
    // ::EndBlendMode();
    for(auto& child : children){
        if (!child->visible) continue;
        child->OnDraw();
    }
}