#include "ui/checkbox.hpp"
#include "ui/clickable.hpp"
#include "ui/label.hpp"

namespace {

class CheckboxBox : public Clickable {
public:
    CheckboxBox(bool* value, std::function<void(bool)> onChange)
        : Clickable(), m_value(value), m_onChange(std::move(onChange)) {}

    void OnDrawContent() override {
        auto rect = GetVisualRect();
        float yCenter = rect.y + rect.height / 2.0f;

        Rectangle box = {
            rect.x,
            yCenter - m_boxSize / 2.0f,
            m_boxSize,
            m_boxSize
        };

        Color boxColor = RAYWHITE;
        if (m_hover && !m_hold)  boxColor = LIGHTGRAY;
        if (m_hover && m_hold)   boxColor = GRAY;

        ::DrawRectangleLinesEx(box, 2, boxColor);

        if (m_value && *m_value){
            float thickness = 3;
            float pad = 4;
            ::DrawLineEx(
                {box.x + pad, box.y + pad},
                {box.x + box.width - pad, box.y + box.height - pad},
                thickness, boxColor
            );
            ::DrawLineEx(
                {box.x + box.width - pad, box.y + pad},
                {box.x + pad, box.y + box.height - pad},
                thickness, boxColor
            );
        }
    }

    void MeasureContent(Vector2) override {
        m_contentDesiredSize = {m_boxSize, m_boxSize};
    }

    bool OnUpdate(float) override { return false; }

protected:
    void OnClick() override {
        if (!m_value) return;
        *m_value = !*m_value;
        if (m_onChange) m_onChange(*m_value);
    }

private:
    bool* m_value;
    std::function<void(bool)> m_onChange;
    static constexpr float m_boxSize = 20.0f;
};

}

Checkbox::Checkbox(
    Text text,
    bool* value,
    std::function<void(bool)> onChange,
    UIComponentSpec spec
): HorizontalLayout{spec, LayoutSpec{Alignment::Center}}
{
    AddChild(std::make_unique<CheckboxBox>(value, std::move(onChange)));
    AddChild(std::make_unique<Label>(std::move(text)));
}
