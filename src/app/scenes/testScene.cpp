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
                    [j](){ dbg::GetLogger().Info("Button"+ std::to_string(j) +" clicked!"); },
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
}

void TestScene::OnResize(){
    Tester tester("OnResize",0,true);
    Vector2 dims = {
        static_cast<float>(::GetScreenWidth()),
        static_cast<float>(::GetScreenHeight())
    };
    MyRectangle rectangle = {{0,0}, dims};
    dbg::GetLogger().LogFormat(dbg::Severity::ERROR, "hey this: {}", dims.x);

    root.OnMeasure(dims);
    root.OnArrange(rectangle);
}
void TestScene::OnDrawContent(){
    PerfTester tester = GetServices().perfLog.log("OnDrawContent");
    ::DrawTexture(m_manager.getTexture("menu"), 0,0,RAYWHITE);
    ::BeginBlendMode(BlendMode::BLEND_ADDITIVE);
    ::DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(),{25,25,25,255});
    ::EndBlendMode();
    root.OnDraw();
}
