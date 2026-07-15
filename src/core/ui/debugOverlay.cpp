#include "core/ui/debugOverlay.hpp"
#include "core/services.hpp"
#include "core/debug.hpp"
#include "core/util.hpp"
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
        UICSpec{}.SetPaddingPct(padBase).SetFlex({1,1}),
        LayoutSpec{}.AlignBegin().CrossBegin().CrossShrink(true)
    );
    auto left = std::make_unique<DebugVerticalLayout>(
        UICSpec{}.SetPaddingPct(padBase).SetFlex({1,1}).FillMaxSize(),
        LayoutSpec{}.AlignBegin().CrossBegin()
    );
    left->Add(
        FPSDraw(Text("", "TNR", fontSize, 0)),
        CursorTrack(Text("", "TNR", fontSize, 0)),
        CfgDisplay(Text("", "TNR", fontSize, 0)),
        DebugLogDisplay(Text("", "TNR", fontSize, 0),UICSpec{}.SetFlex({0,1}).FillMaxSize())
    );
    auto right = std::make_unique<DebugVerticalLayout>(
        UICSpec{}.SetPaddingPct(padBase).SetFlex({1,1}).FillMaxSize(),
        LayoutSpec{}.AlignBegin().CrossEnd());
    right->Add(
            PerformanceDisplay(Text("", "TNR", fontSize, 0),UICSpec{}.SetFlex({0,1})),
            Label(
                Text("W/S bright  E/D contrast\nR/F satur     T/G gamma\nY/H alpha\nF1 overlay  F2 layout\nF3 content F4 fps\nF5 cursor   F6 perf\nF7 log   F8 shader\nF9 gradient", "TNR", fontSize, 0),
                UICSpec{}.SetFlex({0,1}))
    );
    mainC->AddChild(std::move(left));
    mainC->AddChild(std::move(right));

    auto tempBottom = std::make_unique<HorizontalLayout>(
        UICSpec{}.SetPaddingPct(padBase).SetFlex({0.5,2}).FillMaxSize(),
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
    if (GetServices().runtimeCfg.showOverlayGradient) {
        auto r = m_actual;
        ::BeginBlendMode(BlendMode::BLEND_MULTIPLIED);
        auto ir = irect(r); ::DrawRectangleGradientV(ir.x,ir.y,ir.width,ir.height, {0,0,50,100}, {25,0,0,50});
        ::EndBlendMode();
    }
    for(auto& child : m_children){
        if (!child->visible) continue;
        child->OnDraw();
    }
}
bool CfgDisplay::OnUpdate(){
    auto& cfg = GetServices().runtimeCfg;
    auto& p = cfg.processing;
    SetText(std::format(
        "shader: {}\nbrightness: {:.2f}\ncontrast:   {:.2f}\nsaturation: {:.2f}\ngamma:      {:.2f}\nalpha:      {:.2f}",
        cfg.useProcessingShader ? "PROCESSING" : "BRIGHTNESS",
        p.brightness, p.contrast, p.saturation, p.gamma, p.alpha));
    return Label::OnUpdate();
}

void DebugHorizontalLayout::OnDrawContent(){
    if (GetServices().runtimeCfg.showOverlayGradient) {
        auto r = GetDrawRect();
        auto ir2 = irect(r); ::DrawRectangleGradientV(ir2.x,ir2.y,ir2.width,ir2.height, {0,255,0,255}, {0,0,255,0});
    }
    for(auto& child : m_children){
        if (!child->visible) continue;
        child->OnDraw();
    }
}