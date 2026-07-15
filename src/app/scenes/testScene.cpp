#include "app/scenes/testScene.hpp"
#include "app/scenes/customUiComps.hpp"
#include "core/ui/overlay.hpp"
#include "core/util.hpp"
#include "core/ui/debugOverlay.hpp"
#include "core/ui/button.hpp"
#include "core/ui/layout.hpp"
#include "app/scenes/settingsScene.hpp"

TestScene::TestScene(RecourceManager& manager): m_manager{manager}
{}

void TestScene::OnEnter(){
    static int counter{0};
    auto column= std::make_unique<VerticalLayout>(UIComponentSpec{},LayoutSpec{Alignment::End});
    for(int i = 0; i < 1; i++){
        auto buttonText = Text("Test", "TNR", 32, 0, RAYWHITE);
        auto spec = UICSpec().SetFlex({.growth = 1, .shrink = 1})
                    .MinSize({100,50});
        auto row = std::make_unique<HorizontalLayout>(
                    UICSpec{}.FillMaxWidth().SetPaddingPct({.left = 3, .right = 3}),
                    LayoutSpec{}.AlignCenter().CrossCenter().JustifyEvenly().Spacing(20)
                );
        for(int j = 0; j < 4; j++){
            row->Add(
                Button(buttonText,
                    [j](){ dbg::GetLogger().Info(std::to_string(++counter), " Button"+ std::to_string(j) +" clicked!"); },
                    "button_default",{200.f,100.f}, spec)
            );
        }
        row->Add(
            Button(Text("Settings", "TNR", 32, 0, RAYWHITE),
                [](){ 
                    GetServices().sceneManager.QueTransitSus<SettingsScene>();
                },
                "button_default",{200.f,100.f}, spec)
        );
        column->AddChild(std::move(row));    
    }
    root.AddChild(std::move(column));
    root.AddChild(std::make_unique<Overlay>());
    root.AddChild(std::make_unique<DebugOverlay>());
    OnResize();
}
void TestScene::OnRestore(){
    OnResize();
}

void TestScene::OnUpdateState(){
    constexpr float speed = 0.02f;
    auto& input = GetServices().input;
    auto& p = GetServices().runtimeCfg.user.processing;

    p.brightness += input.GetAxis(KEY_W, KEY_S) * speed;
    p.contrast   += input.GetAxis(KEY_E, KEY_D) * speed;
    p.saturation += input.GetAxis(KEY_R, KEY_F) * speed;
    p.gamma      += input.GetAxis(KEY_T, KEY_G) * speed;
    p.alpha      += input.GetAxis(KEY_Y, KEY_H) * speed;

    myClamp(p.brightness, 0.1f, 3.0f);
    myClamp(p.contrast,   0.0f, 5.0f);
    myClamp(p.saturation, 0.0f, 5.0f);
    myClamp(p.gamma,      0.1f, 5.0f);
    myClamp(p.alpha,      0.0f, 1.0f);
}

void TestScene::OnDrawContent(){
    PerfTester tester = GetServices().perfLog.log("OnDrawContent");

    auto& cfg = GetServices().runtimeCfg;
    auto& p = cfg.user.processing;
    auto drawCall = [this](){
        ::DrawTexture(m_manager.getTexture("menu"), 0, 0, RAYWHITE);
    };
    if (cfg.debug.useProcessingShader) {
        useShaderUnchecked(m_manager.getShaderProgram("processing"),
            {{"brightness", p.brightness},
             {"contrast",   p.contrast},
             {"saturation", p.saturation},
             {"gamma",      p.gamma},
             {"alpha",      p.alpha}},
            drawCall);
    } else {
        useShaderUnchecked(m_manager.getShaderProgram("brightness"),
            {{"brightness", p.brightness}},
            drawCall);
    }
    root.OnDraw();
}
