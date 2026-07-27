#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <optional>
#include <type_traits>

#include "ui/clickable.hpp"
#include "utils/animated.hpp"

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
             T step = 0,
             std::optional<T> defaultValue = std::nullopt)
        : Clickable(spec)
        , m_value(value)
        , m_min(min)
        , m_max(max)
        , m_step(step)
        , m_defaultValue(defaultValue)
        , m_onChange(std::move(onChange))
        , m_barHeight(barHeight)
        , m_minThumbSize(minThumbSize)
        , m_maxThumbSize(maxThumbSize)
    {
        this->m_targetSize = targetSize;
        m_visualNorm.setImmediate(valueToNorm());
    }

    bool OnUpdate(float dt) override {
        m_doubleClickTimer += dt;
        m_visualNorm.setTarget(valueToNorm());
        m_thumbScale.setTarget(m_hover ? 1.3f : 1.0f);
        m_visualNorm.update(dt);
        m_thumbScale.update(dt);
        return false;
    }

    void OnDrawContent() override {
        auto rect = GetVisualRect();
        float centerY = rect.y + rect.height / 2.0f;

        float trackY = centerY - m_barHeight / 2.0f;
        ::DrawRectangle(static_cast<int>(rect.x), static_cast<int>(trackY),
                        static_cast<int>(rect.width), static_cast<int>(m_barHeight), GRAY);

        if (!m_value) return;
        float thumbX = rect.x + m_visualNorm.current * rect.width;
        float thumbSize = std::clamp(rect.height, m_minThumbSize, m_maxThumbSize);
        thumbSize *= m_thumbScale.current;
        float thumbRadius = thumbSize / 2.0f;
        thumbX = std::clamp(thumbX, rect.x + thumbRadius, rect.x + rect.width - thumbRadius);

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

    bool OnEvent(const MyEvent& event) override {
        if (auto* e = std::get_if<CursorMoveEvent>(&event)) {
            if (m_hold) {
                updateValueFromPos(e->pos.x);
                return true;
            }
            return false;
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

    void OnClick() override {
        if (m_defaultValue && m_doubleClickTimer < c_doubleClickTime) {
            *m_value = *m_defaultValue;
            m_visualNorm.setImmediate(valueToNorm());
        }
        m_doubleClickTimer = 0.0f;
    }

private:
    static constexpr float c_doubleClickTime{0.3f};

    T* m_value;
    T m_min, m_max;
    T m_step;
    std::optional<T> m_defaultValue;
    float m_doubleClickTimer{0};
    std::function<void(T)> m_onChange;
    float m_barHeight;
    float m_minThumbSize, m_maxThumbSize;
    Animated<float> m_visualNorm{0.0f, 0.06f, Easing::easeOutCubic};
    Animated<float> m_thumbScale{1.0f, 0.10f, Easing::easeOutBack};

    float valueToNorm() const {
        if (!m_value) return 0.0f;
        if (m_max == m_min) return 0.0f;
        return std::clamp(static_cast<float>((*m_value - m_min)) / (m_max - m_min), 0.0f, 1.0f);
    }

    void updateValueFromPos(float mouseX) {
        if (!m_value) return;
        auto rect = GetVisualRect();
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
