#include "app/scenes/testScene.hpp"
#include "app/scenes/customUIComps.hpp"
#include "app/overlay.hpp"
#include "core/util.hpp"
#include "core/debugOverlay.hpp"

TestScene::TestScene(RecourceManager& manager): m_manager{manager}
{}

void TestScene::OnEnter(){
    auto column= std::make_unique<VerticalLayout>(UIComponentSpec{},LayoutSpec{Alignment::End});
    for(int i = 0; i < 1; i++){
        auto row = std::make_unique<HorizontalLayout>(
                    UICSpec{}.FillMaxWidth().SetPaddingPct({.left = 3, .right = 3}),
                    LayoutSpec{}.AlignCenter().CrossCenter().JustifyEvenly().Spacing(20)
                );
        for(int j = 0; j < 5; j++){
            row->Add(
                Button(Text("Test", "TNR", 32, 0, RAYWHITE),
                    [j](){ dbg::GetLogger().DebugInfo("Button"+ std::to_string(j) +" clicked!"); },
                    "button_default",{(float)200,(float)100}, 
                    UICSpec().SetFlex({.growth = 1, .shrink = 1})
                    .MinSize({100,50}))
            );
        }
        column->AddChild(std::move(row));    
    }
    root.AddChild(std::move(column));
    root.AddChild(std::make_unique<Overlay>());
    root.AddChild(std::make_unique<DebugOverlay>());
    OnResize();
}

void TestScene::OnUpdateState(){
    auto& brightness = GetServices().runtimeCfg.brightness;
    auto& input = GetServices().input;
    if (input.IsKeyDown(KEY_W)) brightness += 0.01f;
    if (input.IsKeyDown(KEY_S)) brightness -= 0.01f;
    myClamp(brightness, 0.1f, 3.0f);
}

void TestScene::OnResize(){
    Tester tester("OnResize",1,true);
    Vector2 dims = {
        static_cast<float>(::GetScreenWidth()),
        static_cast<float>(::GetScreenHeight())
    };
    MyRectangle rectangle = {{0,0}, dims};
    root.OnMeasure(dims);
    root.OnArrange(rectangle);
}
void TestScene::OnDrawContent(){
    PerfTester tester = GetServices().perfLog.log("OnDrawContent");

    auto drawCall = [this](){
        ::DrawTexture(m_manager.getTexture("menu"), 0,0,RAYWHITE);
    };
    useShader(m_manager.getShaderProgram("brightness"),{{"brightness", GetServices().runtimeCfg.brightness}},drawCall);
    root.OnDraw();
}
