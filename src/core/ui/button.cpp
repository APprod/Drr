#include "core/ui/button.hpp"
#include "core/services.hpp"

Button::Button(
    Text text,
    std::function<void()> onClick,
    std::string textureName,
    Vector2 targetSize,
    UIComponentSpec spec
): Clickable(spec), m_onClick(std::move(onClick)), m_textureName(std::move(textureName)), m_text(std::move(text)) {
    this->m_targetSize = targetSize;
}

bool Button::OnUpdate(){
    auto r = GetDrawRect();
    m_text.ReMeasure({r.width, r.height});
    if (m_text.IsDirty()) {
        m_text.ClearDirty();
        return true;
    }
    return false;
}

void Button::OnDrawContent(){
    auto& manager = GetServices().recManager;
    auto texture = manager.getTexture(m_textureName);
    auto target = GetDrawRect();
    auto& shader = GetServices().recManager.getShaderProgram("processing");
    float brightness = 1.0f;
    if (m_hover && !m_hold){
        brightness = 1.5f;
    }
    else if (m_hover && m_hold){
        brightness = 0.7f;
    }
    useShaderUnchecked(shader,{{"brightness", brightness}},
            [this, texture, target](){
                ::DrawTexturePro(texture, rect(texture), target, {0,0}, 0.f, RAYWHITE);
                m_text.DrawCentered(target);
            }
    );

}
