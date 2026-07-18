#pragma once
#include <vector>
#include <string>

#include "core/ui/component.hpp"
#include "core/ui/button.hpp"
#include "core/ui/modifier.hpp"
#include "core/ui/layout.hpp"
#include "core/debug.hpp"

template<typename T>
class Dropdown : public Button {
public:
    Dropdown(
        std::vector<std::pair<std::string, T>> items,
        T* selectedValue,
        std::string textureName,
        Vector2 targetSize,
        std::function<void(T)> onChange = nullptr,
        Text labelText = Text("", "button"),
        UIComponentSpec spec = {}
    ): Button{labelText,[](){},textureName,targetSize,spec}, m_items{std::move(items)}, m_selected{selectedValue},
    m_onChange{onChange}{
        m_currentIdx = 0;
        if (selectedValue){
            for (size_t idx{0}; idx < m_items.size(); ++idx){
                if (m_items[idx].second == *selectedValue){
                    m_currentIdx = idx;
                    break;
                }
            }
        } else {
            dbg::GetLogger().Warn("Dropdown: selectedValue is null");
        }
    }
    void closePopup() {
        m_pendingClose = true;
    }

    bool OnUpdate(float dt) override {
        if (m_pendingClose) {
            GetUIContext().RemovePopup(m_popupId);
            m_isOpen = false;
            m_pendingClose = false;
        }
        if (m_text.GetText() != m_items[m_currentIdx].first)
            m_text.SetText(m_items[m_currentIdx].first);
        return Button::OnUpdate(dt);
    }
    void openPopup(){
        if (m_isOpen) closePopup();
        m_popupId = GetUIContext().PushPopup(
            Popup(
                BuildMenu(),
                UICSpec{}
            ).SetAnchor(
                [this](){return this->m_actual;}
            ).ParentSize(true)
        );
        m_isOpen = true;
    }
    void Toggle(){m_isOpen ? closePopup() :openPopup();}
    void OnClick() override { Toggle(); }
    void UpdateSelection(size_t idx){
        *m_selected = m_items[idx].second;
        m_currentIdx = idx;
    }
    void OnDrawContent() override {
        Button::OnDrawContent();
    }
    
    VerticalLayout BuildMenu(){
        auto list = VerticalLayout(UICSpec{},LayoutSpec{});
        Text text = m_text;
        for (size_t i = 0; i < m_items.size(); i++) {
            text.SetText(m_items[i].first);
            list.Add(
                Button(text, 
                    [this, i]() {
                        UpdateSelection(i);
                        if (m_onChange) m_onChange(m_items[i].second);
                        closePopup();
                    },
                    m_textureName,
                    {m_targetSize.x, m_actual.height},
                    UICSpec{}.FillMaxWidth()
                )
            );
        }
        return list;
    }

protected:
    std::vector<std::pair<std::string, T>> m_items;
    T* m_selected;
    std::function<void(T)> m_onChange;
    UICompId m_popupId = 0;
    size_t m_currentIdx{0};
    bool m_isOpen = false;
    bool m_pendingClose{false};
};