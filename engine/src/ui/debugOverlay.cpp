#include "ui/debugOverlay.hpp"
#include "services.hpp"
#include "utils/log.hpp"
#include "utils/util.hpp"
#include "ui/overlay.hpp"

#include "ui/checkbox.hpp"
#include "ui/slider.hpp"
#include "ui/valueLabel.hpp"
#include "ui/hotkeysListener.hpp"

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
    auto& srv = GetServices();
    Text valText("");
    UICSpec sliderSpec;
    auto [sBarH, sMinTh, sMaxTh, sTarget] = std::make_tuple(4.0f, 5.0f, 10.0f, Vector2{200,10});
    left->Add(
        FPSDraw(Text("", "default", "default", 4)),
        CursorTrack(Text(""), UICSpec{}.FillMaxWidth()),
        CfgDisplay(Text("")),
        Checkbox(Text("Layout bounds"), &srv.debugFlags.showLayoutBounds),
        Checkbox(Text("Content bounds"), &srv.debugFlags.showLayoutContentBounds),
        Checkbox(Text("VSYNC"), &srv.userSettings.vsync,
            [](bool vsync){
                mylog::GetLogger().LogFormat(mylog::Severity::DBGINFO, "Switching vsync: {}", vsync);
                if (vsync) {SetWindowState(FLAG_VSYNC_HINT);}
                else {ClearWindowState(FLAG_VSYNC_HINT);}
            }
        ),
        Slider<int>(&srv.userSettings.targetFPS, 0, 240, 
            [](int value){
                mylog::GetLogger().LogFormat(mylog::Severity::DBGINFO, "Switching FPS: {}. srv.userSettings.targetFPS: {}", value, GetServices().userSettings.targetFPS);
                SetTargetFPS(value);
            }
            , sliderSpec, sBarH, sMinTh, sMaxTh, sTarget, 5),
        ValueLabel<int>("Target FPS: {}", &srv.userSettings.targetFPS, valText),
        DebugLogDisplay(Text(""),UICSpec{}.SetFlex({0,1}).FillMaxSize())
    );
    auto right = std::make_unique<DebugVerticalLayout>(
        UICSpec{}.SetPaddingPct(padBase).SetFlex({1,1}).FillMaxSize(),
        LayoutSpec{}.AlignBegin().CrossEnd());

    right->Add(
        PerformanceDisplay(valText, UICSpec{}.SetFlex({0,1})),
        ValueLabel<float>("Brightness: {:.2f}", &srv.userSettings.userBrightness, valText),
        Slider<float>(&srv.userSettings.userBrightness, 0.1f, 3.0f, nullptr, sliderSpec, sBarH, sMinTh, sMaxTh, sTarget),
        ValueLabel<int>("Log msgs: {}", &srv.debugFlags.debugMessagesCount, valText),
        Slider<int>(&srv.debugFlags.debugMessagesCount, 1, 50, nullptr, sliderSpec, sBarH, sMinTh, sMaxTh, sTarget),
        BindingsDisplay(Text(""), UICSpec{}.SetFlex({0,1}))
    );
    mainC->AddChild(std::move(left));
    mainC->AddChild(std::move(right));

    auto tempBottom = std::make_unique<HorizontalLayout>(
        UICSpec{}.SetPaddingPct(padBase).SetFlex({0.2f,5.0f}).FillMaxSize(),
        LayoutSpec{}.AlignBegin().CrossBegin()
    );
    temp->AddChild(std::move(mainC));
    temp->AddChild(std::move(tempBottom));
    AddChild(std::move(temp));
}

bool DebugOverlay::OnUpdate(float dt){
    visible = GetServices().debugFlags.showDebugOverlay;
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
    if (GetServices().debugFlags.showCursorPos)
        SetText(std::format("Cursor pos: {:04d} {:04d}",
            static_cast<int>(m_pos.x), static_cast<int>(m_pos.y)));
    else
        SetText("");
    return Label::OnUpdate(dt);
}

bool PerformanceDisplay::OnUpdate(float dt){
    if (!GetServices().debugFlags.showPerformance) {
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
    if (!GetServices().debugFlags.showDebugLog) {
        SetText(""); return Label::OnUpdate(dt);
    }
    int count = GetServices().debugFlags.debugMessagesCount;
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

bool BindingsDisplay::OnUpdate(float dt){
    auto* overlay = dynamic_cast<Overlay*>(GetUIContext().GetOverlay());
    if (!overlay) { SetText(""); return Label::OnUpdate(dt); }
    std::string out;
    for (auto& [key, binding] : overlay->GetBindings()) {
        auto keyStr = GetInputKeyName(key);
        if (!binding.description.empty())
            out += std::format("{}: {}\n", keyStr, binding.description);
        else
            out += std::format("{}\n", keyStr);
    }
    if (!out.empty()) out.pop_back();
    SetText(out);   
    return Label::OnUpdate(dt);
}

void DebugVerticalLayout::OnDrawContent(){
    if (GetServices().debugFlags.showOverlayGradient) {
        auto r = GetActualRect();
        GetServices().renderer.beginBlendMode(BlendMode::BLEND_MULTIPLIED);
        auto ir = irect(r); ::DrawRectangleGradientV(ir.x,ir.y,ir.width,ir.height, {0,0,50,100}, {25,0,0,50});
        GetServices().renderer.endBlendMode();
    }
    for(auto& child : m_children){
        if (!child->visible) continue;
        child->OnDraw();
    }
}
bool CfgDisplay::OnUpdate(float dt){
    auto& srv = GetServices();
    SetText(std::format("brightness: {:.2f}", srv.userSettings.userBrightness));
    return Label::OnUpdate(dt);
}

void DebugHorizontalLayout::OnDrawContent(){
    if (GetServices().debugFlags.showOverlayGradient) {
        auto r = GetVisualRect();
        auto ir2 = irect(r); ::DrawRectangleGradientV(ir2.x,ir2.y,ir2.width,ir2.height, {0,255,0,255}, {0,0,255,0});
    }
    for(auto& child : m_children){
        if (!child->visible) continue;
        child->OnDraw();
    }
}