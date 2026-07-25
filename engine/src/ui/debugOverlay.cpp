#include "ui/debugOverlay.hpp"
#include "services.hpp"
#include "utils/log.hpp"
#include "utils/util.hpp"
#include "ui/label.hpp"
#include <format>

DebugOverlay::DebugOverlay(UIComponentSpec uiSpec, LayoutSpec layoutSpec)
: Stack(uiSpec, layoutSpec) {
    
    recievesEvents = true;
    
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
    auto& cfg = GetServices().runtimeCfg;
    auto& p = cfg.user.processing;
    Text valText("", "text");
    UICSpec sliderSpec;
    auto [sBarH, sMinTh, sMaxTh, sTarget] = std::make_tuple(4.0f, 5.0f, 10.0f, Vector2{200,10});
    left->Add(
        FPSDraw(Text("", "big header")),
        CursorTrack(Text("", "text")),
        CfgDisplay(Text("", "text")),
        Checkbox(Text("Layout bounds", "text"), &cfg.debug.showLayoutBounds),
        Checkbox(Text("Content bounds", "text"), &cfg.debug.showLayoutContentBounds),
        Checkbox(Text("VSYNC", "text"), &cfg.user.vsync,
            [](bool vsync){
                mylog::GetLogger().LogFormat(mylog::Severity::DBGINFO, "Switching vsync: {}", vsync);
                if (vsync) {SetWindowState(FLAG_VSYNC_HINT);}
                else {ClearWindowState(FLAG_VSYNC_HINT);}
            }
        ),
        Slider<int>(&cfg.user.targetFPS, 0, 240, 
            [](int value){
                mylog::GetLogger().LogFormat(mylog::Severity::DBGINFO, "Switching FPS: {}. cfg.user.targetFPS: {}", value, GetServices().runtimeCfg.user.targetFPS);
                SetTargetFPS(value);
            }
            , sliderSpec, sBarH, sMinTh, sMaxTh, sTarget, 5),
        ValueLabel<int>("Target FPS: {}", &cfg.user.targetFPS, valText),
        DebugLogDisplay(Text("", "text"),UICSpec{}.SetFlex({0,1}).FillMaxSize())
    );
    auto right = std::make_unique<DebugVerticalLayout>(
        UICSpec{}.SetPaddingPct(padBase).SetFlex({1,1}).FillMaxSize(),
        LayoutSpec{}.AlignBegin().CrossEnd());

    right->Add(
        PerformanceDisplay(valText, UICSpec{}.SetFlex({0,1})),
        ValueLabel<float>("Brightness: {:.2f}", &p.brightness, valText),
        Slider<float>(&p.brightness, 0.1f, 3.0f, nullptr, sliderSpec, sBarH, sMinTh, sMaxTh, sTarget),
        ValueLabel<float>("Contrast: {:.2f}", &p.contrast, valText),
        Slider<float>(&p.contrast, 0.0f, 5.0f, nullptr, sliderSpec, sBarH, sMinTh, sMaxTh, sTarget),
        ValueLabel<float>("Saturation: {:.2f}", &p.saturation, valText),
        Slider<float>(&p.saturation, 0.0f, 5.0f, nullptr, sliderSpec, sBarH, sMinTh, sMaxTh, sTarget),
        ValueLabel<float>("Gamma: {:.2f}", &p.gamma, valText),
        Slider<float>(&p.gamma, 0.1f, 5.0f, nullptr, sliderSpec, sBarH, sMinTh, sMaxTh, sTarget),
        ValueLabel<float>("Alpha: {:.2f}", &p.alpha, valText),
        Slider<float>(&p.alpha, 0.0f, 1.0f, nullptr, sliderSpec, sBarH, sMinTh, sMaxTh, sTarget),
        ValueLabel<int>("Log msgs: {}", &cfg.debug.debugMessagesCount, valText),
        Slider<int>(&cfg.debug.debugMessagesCount, 1, 50, nullptr, sliderSpec, sBarH, sMinTh, sMaxTh, sTarget),
        Label(
            Text("F1 overlay  F2 layout\nF3 content  F4 fps\nF5 cursor   F6 perf\nF7 log   F8 shader\nF9 gradient", "text"),
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

bool DebugOverlay::OnUpdate(float dt){
    visible = GetServices().runtimeCfg.debug.showDebugOverlay;
    hitTesting = recievesEvents = visible;
    return Stack::OnUpdate(dt);
}


bool CursorTrack::OnEvent(const MyEvent& event){
    if (std::holds_alternative<CursorMoveEvent>(event)){
        const auto& pos = std::get<CursorMoveEvent>(event).pos;
        m_pos = pos;
    }
    return false;
}


bool CursorTrack::OnUpdate(float dt){
    if (GetServices().runtimeCfg.debug.showCursorPos)
        SetText(std::format("Cursor pos: {:04d} {:04d}",
            static_cast<int>(m_pos.x), static_cast<int>(m_pos.y)));
    else
        SetText("");
    return Label::OnUpdate(dt);
}

bool PerformanceDisplay::OnUpdate(float dt){
    if (!GetServices().runtimeCfg.debug.showPerformance) {
        SetText(""); return Label::OnUpdate(dt);
    }
    std::string out;
    for (auto& [name, stat] : GetServices().perfLog.getLogData()) {
        out += std::format("{}: avg={:.2f}ms pk={:.2f}ms\n", name, stat.average(), stat.peak());
    }
    SetText(out);
    return Label::OnUpdate(dt);
}

bool DebugLogDisplay::OnUpdate(float dt){
    if (!GetServices().runtimeCfg.debug.showDebugLog) {
        SetText(""); return Label::OnUpdate(dt);
    }
    int count = GetServices().runtimeCfg.debug.debugMessagesCount;
    const auto& messages = mylog::GetLogger().GetMessages();
    std::string out;
    int shown = 0;
    for (int i = static_cast<int>(messages.size()) - 1; i >= 0 && shown < count; --i) {
        // if (messages[i].severity == dbg::Severity::DBGINFO) continue;
        out = std::format("{}: {}\n", mylog::ToString(messages[i].severity), messages[i].message) + out;
        ++shown;
    }
    SetText(out);
    return Label::OnUpdate(dt);
}



void DebugVerticalLayout::OnDrawContent(){
    if (GetServices().runtimeCfg.debug.showOverlayGradient) {
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
bool CfgDisplay::OnUpdate(float dt){
    auto& cfg = GetServices().runtimeCfg;
    auto& p = cfg.user.processing;
    SetText(std::format(
        "shader: {}\nbrightness: {:.2f}\ncontrast:   {:.2f}\nsaturation: {:.2f}\ngamma:      {:.2f}\nalpha:      {:.2f}",
        cfg.debug.useProcessingShader ? "PROCESSING" : "BRIGHTNESS",
        p.brightness, p.contrast, p.saturation, p.gamma, p.alpha));
    return Label::OnUpdate(dt);
}

void DebugHorizontalLayout::OnDrawContent(){
    if (GetServices().runtimeCfg.debug.showOverlayGradient) {
        auto r = GetDrawRect();
        auto ir2 = irect(r); ::DrawRectangleGradientV(ir2.x,ir2.y,ir2.width,ir2.height, {0,255,0,255}, {0,0,255,0});
    }
    for(auto& child : m_children){
        if (!child->visible) continue;
        child->OnDraw();
    }
}