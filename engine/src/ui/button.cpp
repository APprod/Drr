#include "ui/button.hpp"
#include "services.hpp"

Button::Button(
    Text text,
    std::function<void()> onClick,
    TextureSpec textureSpec,
    Vector2 targetSize,
    UIComponentSpec spec
): Clickable(spec), m_onClick(std::move(onClick)), m_textureSpec(std::move(textureSpec)), m_text(std::move(text)) {
    this->m_targetSize = targetSize;
}

void Button::MeasureContent(Vector2 available) {
    auto textSize = m_text.ReMeasure(available);
    m_contentDesiredSize = {
        std::min(available.x, std::max(m_targetSize.x, textSize.x)),
        std::min(available.y, std::max(m_targetSize.y, textSize.y))
    };
}

bool Button::OnUpdate(float dt){
    auto r = GetVisualRect();
    m_text.ReMeasure({r.width, r.height});
    if (m_text.IsDirty()) {
        m_text.ClearDirty();
        return true;
    }

    float targetBrightness = 1.0f;
    float targetScale = 1.0f;
    if (m_hover && !m_hold) {
        targetBrightness = 1.5f;
        targetScale = 1.05f;
    } else if (m_hold) {
        targetBrightness = 0.7f;
    }
    m_brightness.setTarget(targetBrightness);
    m_scale.setTarget(targetScale);

    m_brightness.update(dt);
    m_scale.update(dt);

    return false;
}

Padding Button::ResolvePadding(Vector2 dims) const {
    auto pad = Clickable::ResolvePadding(dims);
    if (m_textureSpec.slice) {
        pad.left += m_textureSpec.slice->left;
        pad.top += m_textureSpec.slice->top;
        pad.right += m_textureSpec.slice->right;
        pad.bottom += m_textureSpec.slice->bottom;
    }
    return pad;
}

void Button::OnDrawContent(){
    auto target = Clickable::GetActualRect();
    float s = m_scale.current;
    if (s != 1.0f) {
        target = {
            target.x + target.width * (1.0f - s) * 0.5f,
            target.y + target.height * (1.0f - s) * 0.5f,
            target.width * s,
            target.height * s,
        };
    }

    m_textureSpec.processing = BrightnessSh(m_brightness.current);
    m_textureSpec.Draw(target);

    m_text.DrawCentered(GetVisualRect());
}
