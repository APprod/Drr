#include "core/scene.hpp"
#include "core/ui.hpp"
#include "core/recManager.hpp"
#include "core/util.hpp"

class TestScene: public IScene{
public:
    TestScene(RecourceManager& manager);
    void OnEnter() override;
    virtual void OnRestore() override;
    virtual void OnUpdate() override;
    void OnResize();
    virtual void OnDraw() override;
    virtual void OnExit() override; 
    virtual void OnSuspend() override;
private:
    Root root;
    RecourceManager& m_manager;
};