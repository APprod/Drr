#include "app/scenes/testScene.hpp"
#include "app/scenes/customUIComps.hpp"

TestScene::TestScene(RecourceManager& manager): m_manager{manager}
{}

void TestScene::OnEnter(){
    auto row= std::make_unique<HorizontalLayout>();
    for(int i = 0; i < 10; i++){
        auto column = std::make_unique<VerticalLayout>();
        for(int j = 0; j < 10; j++){
            column->AddChild(std::make_unique<TestComp>(Vector2{100,50}));
        }
        row->AddChild(std::move(column));    
    }
    root.AddChild(std::move(row));
    OnResize();
}

void TestScene::OnRestore(){}

void TestScene::OnUpdate(){
    root.OnUpdate();
    if (::IsWindowResized()){
        OnResize();
    }
}

void TestScene::OnResize(){
    Tester tester("OnResize",0,true);
    Vector2 dims = {
        static_cast<float>(::GetScreenWidth()),
        static_cast<float>(::GetScreenHeight())
    };
    Rectangle rect = {0,0,
        static_cast<float>(::GetScreenWidth()),
        static_cast<float>(::GetScreenHeight())
    };
    root.OnMeasure(dims);
    root.OnArrange(rect);
}
void TestScene::OnDraw(){
    BeginDrawing();
        ClearBackground(BLACK); 
        
        DrawTexture(m_manager.getTexture("menu"), 0,0,RAYWHITE);
        root.OnDraw();
        DrawFPS(20, 20);
    EndDrawing();  
}

void TestScene::OnExit(){}

void TestScene::OnSuspend(){}