#pragma once

class IScene{ //Base class for Scene
public:
    virtual ~IScene() = default; 

    // Called on Scene first creation
    // Can be used to load assets and build UI layout
    virtual void OnEnter() = 0; 
    // Called only if the scene was suspended with QueTransitSus and returned back to with QuePop
    virtual void OnRestore() = 0; 

    // Called every frame before OnDraw
    // Do all the input + state update here
    virtual void OnUpdate(float dt) = 0;
    virtual void OnDraw() = 0; 

    // Called repeatedly when the scene was first created or returned to
    // should return true if the animation was finished
    virtual bool AnimateEnter(float ) { return true; } 
    // Called repeatedly when any kind of transitin happens to different scene 
    // should return true if the animation was finished Update transiting animations and other work here
    // All other stages in scene update pipeline STILL HAPPEN until this finishes
    virtual bool AnimateExit(float )  { return true; }

    // Unload all the loaded assets and do all needed processing
    // Called when transiting destroys current scene
    virtual void OnExit() = 0;
    // Called when transiting from this scene with suspension(it is kept in the stack)
    virtual void OnSuspend() = 0;
private:
};
