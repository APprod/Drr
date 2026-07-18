#include "core/ui/root.hpp"
#include "core/util.hpp"

#include <stdexcept>

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

Vector2 Root::getPos(const MyEvent& event){
    Vector2 res = std::visit(overloaded{
        [](const CursorMoveEvent& e) -> Vector2 {return e.pos;},
        [](const CursorActionEvent& e) -> Vector2 {return e.pos;},
        [](const auto&) -> Vector2 {throw std::runtime_error("Required to get a position of an event without the position");}
    }, event);
    return res;
}

void Root::UpdateHover()
{
    UIComponent* newHovered = nullptr;
    auto captured = GetUIContext().GetCapturered();
    if (captured){
        if (captured->HitTest(m_cursorPos))
            newHovered = captured;
    }
    else{
        newHovered = FindTarget(m_cursorPos);
    }
    auto hovered = GetUIContext().GetHovered();
    if (newHovered == hovered)
        return;
    if (hovered)
    {
        dbg::GetLogger().DebugInfo("OnHoverExit on: ", hovered->id);
        hovered->OnHoverExit();
    }

    GetUIContext().SetHovered(newHovered);
    if (newHovered)
        newHovered->OnHoverEnter();
}

std::optional<bool> Root::CheckCaptured(const MyEvent& event){
    auto captured = GetUIContext().GetCapturered();
    if (!captured)
        return std::nullopt;
    if (!(captured->getCaptureTypes() & getEventType(event)))
        return std::nullopt;

    bool result = captured->OnEvent(event);
    UpdateHover();
    return result;
}

bool Root::OnEvent(const MyEvent& event){
    PerfTester tester = GetServices().perfLog.log("Root::OnEvent");

    if (auto* move = std::get_if<CursorMoveEvent>(&event)){
        m_cursorPos = move->pos;
    }
    else if (auto* screen = std::get_if<ScreenInterEvent>(&event)){
        if (screen->action == ScreenInteraction::EXIT)
        {
            auto hovered = GetUIContext().GetHovered();
            if (hovered)
            {
                hovered->OnHoverExit();
                GetUIContext().SetHovered(nullptr);
            }
        }
        return true;
    }
    { //Always
        if (auto r = CheckCaptured(event)) return *r;
        bool result = Layout::OnEvent(event);
        UpdateHover();
        return result;
    }
}
