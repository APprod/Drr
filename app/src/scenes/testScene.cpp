#include "scenes/testScene.hpp"
#include "scenes/customUiComps.hpp"
#include "ui/overlay.hpp"
#include "utils/util.hpp"
#include "ui/debugOverlay.hpp"
#include "ui/button.hpp"
#include "ui/layout.hpp"
#include "scenes/settingsScene.hpp"
#include "ui/modifier.hpp"
#include "ui/dropdown.hpp"

TestScene::TestScene()
{
    m_transitionTime = 0.2f;
}

void TestScene::OnEnter(){
    static int counter{0};
    auto column= std::make_unique<VerticalLayout>(UIComponentSpec{},LayoutSpec{Alignment::End});
    for(int i = 0; i < 1; i++){
        auto buttonText = Text("Test", "button");
        auto spec = UICSpec().SetFlex({.growth = 1, .shrink = 1})
                    .MinSize({100,50});
        auto row = std::make_unique<HorizontalScrollView>(
                    UICSpec{}.FillMaxWidth().SetPaddingPct({.left = 3, .right = 3}),
                    LayoutSpec{}.AlignCenter().CrossCenter().JustifyEvenly().Spacing(20)
                );
        for(int j = 0; j < 4; j++){
            row->Add(
                Button(buttonText,
                    [j](){ mylog::GetLogger().Info(std::to_string(++counter), " Button"+ std::to_string(j) +" clicked!"); },
                    TextureSpec("button_default"),{200.f,100.f}, spec)
            );
        }
        row->Add(
            Button(Text("Settings", "button"),
                [](){ 
                    GetServices().sceneManager.QueTransitSus<SettingsScene>();
                },
                TextureSpec("button_default"),{200.f,100.f}, spec)
        );
        column->AddChild(std::move(row));    
    }
    root.AddChild(std::move(column));
    root.AddChild(std::make_unique<Overlay>());
    root.AddChild(std::make_unique<DebugOverlay>());
    
    OnResize();
}
void TestScene::OnRestore(){
    BaseScene::OnRestore();
    OnResize();
}

void TestScene::OnUpdateState(){
}

void TestScene::OnDrawContent(){
    PerfTester tester = GetServices().perfLog.log("OnDrawContent");
    auto& manager = GetServices().resManager;
    ::DrawTexture(manager.getTexture("menu"), 0, 0, RAYWHITE);
    root.OnDraw();
    BaseScene::DrawFadeTransition();
    
}
