#include "core/ui/button.hpp"
#include "core/services.hpp"

Button::Button(
    Text text,
    std::function<void()> onClick,
    std::string textureName,
    Vector2 targetSize,
    UIComponentSpec spec
): UIComponent{spec}, m_text{std::move(text)}, m_onClick{onClick}, m_textureName{textureName} {
    this->targetSize = targetSize;
}

void Button::OnDrawContent(){
    auto& manager = GetServices().recManager;
    auto texture = manager.getTexture(m_textureName);
    auto target = GetDrawRect();
    ::DrawTexturePro(texture, rect(texture), target, {0,0}, 0.f, RAYWHITE);

    m_text.DrawCentered(target);
    if (m_hover && !m_hold){
        ::DrawRectanglePro(target,{0,0},0.f,{255,255,255,50});
    }
    else if (m_hover && m_hold){
        ::BeginBlendMode(BlendMode::BLEND_MULTIPLIED);
        ::DrawRectanglePro(target,{0,0},0.f,{150,150,150,255});
        ::EndBlendMode();
    }
}

EventResult Button::OnEvent(const MyEvent& event){
    if (std::holds_alternative<CursorActionEvent>(event)){
        const auto& btn = std::get<CursorActionEvent>(event);
        if (btn.button == CursorAction::MOUSE_BUTTON_LEFT && btn.pressed){
            if (HitTest(btn.pos)){
                m_hold = true;
                return EventResult::RequireCapture;
            }
        }
        if (btn.button == CursorAction::MOUSE_BUTTON_LEFT && !btn.pressed){
            if (m_hold){
                if (HitTest(btn.pos)){
                    m_onClick();
                }
                m_hold = false;
                return EventResult::ReleaseCapture;
            }

        }
    }
    return EventResult::NotHandled;
}

void Button::OnHoverEnter()
{
    m_hover = true;
}

void Button::OnHoverExit()
{
    m_hover = false;
}
