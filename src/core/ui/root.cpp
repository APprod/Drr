#include "core/ui/root.hpp"
#include "core/util.hpp"

#include <stdexcept>

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

Vector2 Root::getPos(const MyEvent& event){
    Vector2 res = std::visit(overloaded{
        [](const CursorMoveEvent& e) -> Vector2 {return e.pos;},
        [](const CursorActionEvent& e) -> Vector2 {return e.pos;},
        [](const auto&){throw std::runtime_error("Required to get a position of an event without the position"); return Vector2{0,0};}
    }, event);
    return res;
}

void Root::UpdateHover()
{
    UIComponent* newHovered = nullptr;

    if (m_captured){
        if (m_captured->HitTest(m_cursorPos))
            newHovered = m_captured;
    }
    else{
        newHovered = FindTarget(m_cursorPos);
    }

    if (newHovered == m_hovered)
        return;
    if (m_hovered)
        m_hovered->OnHoverExit();

    m_hovered = newHovered;
    if (m_hovered)
        m_hovered->OnHoverEnter();
}

std::optional<EventResult> Root::CheckCaptured(const MyEvent& event){
    if (!m_captured)
        return std::nullopt;
    if (!(m_captured->getCaptureTypes() & getEventType(event)))
        return std::nullopt;

    EventResult result = m_captured->OnEvent(event);
    if (result == EventResult::ReleaseCapture)
    {
        m_captured = nullptr;
        UpdateHover();
    }
    return result;
}

EventResult Root::OnEvent(const MyEvent& event){
    PerfTester tester = GetServices().perfLog.log("Root::OnEvent");

    if (auto* move = std::get_if<CursorMoveEvent>(&event))
    {
        m_cursorPos = move->pos;
        if (auto r = CheckCaptured(event))
            return *r;
        Layout::OnEvent(event);
        UpdateHover();
        return EventResult::Handled;
    }

    if (auto* screen = std::get_if<ScreenInterEvent>(&event))
    {
        if (screen->action == ScreenInteraction::EXIT)
        {
            if (m_hovered)
            {
                m_hovered->OnHoverExit();
                m_hovered = nullptr;
            }
        }

        return EventResult::Handled;
    }
    {
        if (auto r = CheckCaptured(event)) return *r;
        EventResult result = Layout::OnEvent(event);
        if (result == EventResult::RequireCapture)
        {
            m_captured = FindTarget(getPos(event));
            UpdateHover();
        }
        return result;
    }
}
