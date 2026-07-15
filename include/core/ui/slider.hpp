#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <type_traits>

#include "core/ui/clickable.hpp"

template<typename T>
requires std::is_arithmetic_v<T>
class Slider : public Clickable {
public:
    Slider(T* value, T min, T max,
           std::function<void(T)> onChange = nullptr,
           UIComponentSpec spec = {},
            float barHeight = 4.0f,
            float minThumbSize = 10.0f,
            float maxThumbSize = 30.0f,
            Vector2 targetSize = {10,10},
            T step = 0)
        : Clickable(spec)
        , m_value(value)
        , m_min(min)
        , m_max(max)
        , m_step(step)
        , m_onChange(std::move(onChange))
        , m_barHeight(barHeight)
        , m_minThumbSize(minThumbSize)
        , m_maxThumbSize(maxThumbSize)
    {
        this->m_targetSize = targetSize;
    }

    void OnDrawContent() override {
        auto rect = GetDrawRect();
        float centerY = rect.y + rect.height / 2.0f;

        float trackY = centerY - m_barHeight / 2.0f;
        ::DrawRectangle(static_cast<int>(rect.x), static_cast<int>(trackY),
                        static_cast<int>(rect.width), static_cast<int>(m_barHeight), GRAY);

        if (!m_value) return;
        float norm = valueToNorm();
        float thumbX = rect.x + norm * rect.width;
        float thumbSize = std::clamp(rect.height, m_minThumbSize, m_maxThumbSize);

        Color thumbColor = RAYWHITE;
        if (m_hover && !m_hold)  thumbColor = LIGHTGRAY;
        if (m_hover && m_hold)   thumbColor = GRAY;

        ::DrawCircle(static_cast<int>(thumbX), static_cast<int>(centerY),
                     thumbSize / 2.0f, thumbColor);
    }

    void MeasureContent(Vector2 available) override {
        float thumbSize = std::clamp(available.y, m_minThumbSize, m_maxThumbSize);
        m_contentDesiredSize = {
            std::min(m_targetSize.x, available.x),
            thumbSize
        };
    }

    EventResult OnEvent(const MyEvent& event) override {
        if (auto* e = std::get_if<CursorMoveEvent>(&event)) {
            if (m_hold) {
                updateValueFromPos(e->pos.x);
                return EventResult::Handled;
            }
            return EventResult::NotHandled;
        }

        if (auto* e = std::get_if<CursorActionEvent>(&event)) {
            if (e->button == CursorAction::MOUSE_BUTTON_LEFT && e->pressed) {
                if (HitTest(e->pos)) {
                    updateValueFromPos(e->pos.x);
                }
            }
        }

        return Clickable::OnEvent(event);
    }

    EventMask getCaptureTypes() const override {
        return EventType::CursorAction | EventType::CursorMove;
    }

protected:
    void OnReleased() override {
        if (m_onChange && m_value) {
            m_onChange(*m_value);
        }
    }

private:
    T* m_value;
    T m_min, m_max;
    T m_step;
    std::function<void(T)> m_onChange;
    float m_barHeight;
    float m_minThumbSize, m_maxThumbSize;

    float valueToNorm() const {
        if (!m_value) return 0.0f;
        if (m_max == m_min) return 0.0f;
        return static_cast<float>((*m_value - m_min)) / (m_max - m_min);
    }

    void updateValueFromPos(float mouseX) {
        if (!m_value) return;
        auto rect = GetDrawRect();
        if (rect.width <= 0.0f) return;

        float norm = (mouseX - rect.x) / rect.width;
        norm = std::clamp(norm, 0.0f, 1.0f);

        if constexpr (std::is_floating_point_v<T>) {
            *m_value = static_cast<T>(m_min + norm * (m_max - m_min));
        } else {
            *m_value = static_cast<T>(std::round(m_min + norm * (m_max - m_min)));
        }
        if (m_step > 0) {
            *m_value = static_cast<T>(std::round((*m_value - m_min) / m_step) * m_step + m_min);
            *m_value = std::clamp(*m_value, m_min, m_max);
        }
    }
};
