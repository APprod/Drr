#pragma once


#include "core/services.hpp"
#include "core/debug.hpp"
#include "core/ui/root.hpp"
#include "core/util.hpp"

class IScene{ //Base class for Scene
public:
    virtual ~IScene() = default; 

    virtual void OnEnter() = 0; //Allocate everything
    virtual void OnRestore() = 0; //Called only if it was Suspended

    virtual void OnUpdate() = 0;
    virtual void OnDraw() = 0; 

    virtual void OnExit() = 0; //DeAllocate, called on normal transit
    virtual void OnSuspend() = 0;  // called on suspended transit
private:
};

class BaseScene: public IScene{
public:
    void OnRestore() override {};
    void OnSuspend() override {};
    void OnExit() override {};

    void OnDraw() override {
        BeginDrawing();
        ClearBackground(BLACK);
        OnDrawContent();
        EndDrawing();
    };
    void OnUpdate() override {
        PerfTester tester = GetServices().perfLog.log("Scene OnUpdate Full");
        auto& input = GetServices().input;
        {
            PerfTester innerTester = GetServices().perfLog.log("Scene input.pollEvents");
            input.pollEvents();
        }
        auto events = input.getEvents();
        for (auto& event: events){
            root.OnEvent(event); //updates states
        }
        bool needsResize = root.OnUpdate(); //updates other data related to UI states
        if (needsResize || ::IsWindowResized()) { //resize if needed
            OnResize();
        }
        OnUpdateState(); //updates scene data not UI
    };
protected:
    virtual void OnResize(){
        dbg::GetLogger().DebugInfo("Resized: ", Vector2{::GetScreenWidth()/1.0f, ::GetScreenHeight()/1.0f});
        PerfTester tester = GetServices().perfLog.log("OnResize");
        auto dims = Vector2{
                static_cast<float>(::GetScreenWidth()),
                static_cast<float>(::GetScreenHeight())
            };
            MyRectangle rect = {{0,0}, dims};
            root.OnMeasure(dims);
            root.OnArrange(rect);
    }
    virtual void OnDrawContent() = 0;
    virtual void OnUpdateState() = 0;
    Root root;
};
