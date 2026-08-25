#pragma once

#include <functional>
#include <string>

#include "ui/label.hpp"

class TextInputField : public Label {
public:
    TextInputField(
        std::string role = "default",
        UIComponentSpec spec = {},
        std::function<void(std::string)> onSubmit = nullptr
    );

    void SetText(std::string text);
    const std::string& GetText() const { return m_buffer; }

    bool OnUpdate(float dt) override;
    bool OnEvent(const MyEvent& event) override;
    void MeasureContent(Vector2 available) override;
    void OnDrawContent() override;
    EventMask getCaptureTypes() const override {
        return EventType::InputKey | EventType::CursorAction | EventType::CharInput;
    }

protected:
    void insertChar(int codepoint);
    void deleteBefore();
    void deleteAfter();
    void cursorLeft();
    void cursorRight();
    void cursorHome();
    void cursorEnd();

    void focus();
    void blur();
    void syncText();
    float cursorX() const;
    void updateViewShift();
    void cursorToPos(float x);

    std::string m_buffer;
    std::function<void(std::string)> m_onSubmit;
    int m_cursorPos{0};
    bool m_focused{false};
    float m_cursorTimer{0};
    float m_viewShift{0};
};
