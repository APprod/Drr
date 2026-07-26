#pragma once
#include <vector>
#include <string>

#include "ui/component.hpp"
#include "ui/button.hpp"
#include "ui/modifier.hpp"
#include "ui/layout.hpp"
#include "utils/log.hpp"

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
        //Set starting state
        m_currentIdx = 0;
        if (selectedValue){
            for (size_t idx{0}; idx < m_items.size(); ++idx){
                if (m_items[idx].second == *selectedValue){
                    m_currentIdx = idx;
                    break;
                }
            }
        } else {
            mylog::GetLogger().Warn("Dropdown: selectedValue is null");
        }
    }
    ~Dropdown() {
        if (m_popupId != 0)
            GetUIContext().RemovePopupImmediate(m_popupId); 
        // All lambdas that captured id are destroyed
    }
    Dropdown(Dropdown&& other) noexcept
    : Button(std::move(other)),
      m_items(std::move(other.m_items)),
      m_selected(other.m_selected),
      m_onChange(std::move(other.m_onChange)),
      m_popupId(other.m_popupId),
      m_currentIdx(other.m_currentIdx),
      m_isOpen(other.m_isOpen)
    {
        other.m_popupId = 0;
    }
    Dropdown& operator=(Dropdown&& other) noexcept {
        if (this == &other) return *this;
        if (m_popupId != 0)
            GetUIContext().RemovePopupImmediate(m_popupId);
        Button::operator=(std::move(other));
        m_items = std::move(other.m_items);
        m_selected = other.m_selected;
        m_onChange = std::move(other.m_onChange);
        m_popupId = other.m_popupId;
        m_currentIdx = other.m_currentIdx;
        m_isOpen = other.m_isOpen;
        
        other.m_popupId = 0;
        
        return *this;
    }
    Dropdown(Dropdown&) = delete;
    Dropdown& operator=(Dropdown&) = delete;
    void closePopup() {
        if (auto* reveal = GetReveal()){
            reveal->setOpen(false);
            m_isOpen = false;
        }
    }
    AnimatedReveal* GetReveal(){
        if (auto* comp = GetUIContext().GetPopupById(m_popupId)){
            auto* popup = comp->GetAs<Popup>();
            if (!popup) {mylog::GetLogger().Error("Popup of dropdown lost(cant cast to it dynamically)"); return nullptr;}
            auto* reveal = popup->GetChildAs<AnimatedReveal>();
            if (!reveal) {mylog::GetLogger().Error("Animated reveal of dropdown popup lost(cant cast to it dynamically)"); return nullptr;}
            return reveal;
        }
        return nullptr;
    }
    void openPopup() {
        if (auto* reveal = GetReveal()){
            reveal->setOpen(true);
            m_isOpen = true;
            return;
        }

        OpenDirection dir = OpenDirection::Down;
        { //Direction calculation fine
            auto actual = GetActualRect();
            float estimatedHeight = static_cast<float>(m_items.size()) * actual.height;
            float spaceBelow = static_cast<float>(GetScreenHeight()) - (actual.y + actual.height);
            float spaceAbove = actual.y;
            if (spaceBelow < estimatedHeight && spaceAbove >= estimatedHeight)
                dir = OpenDirection::Up;
        }

        auto reveal = AnimatedReveal(
            BuildMenu(), UICSpec{},
            Animated<float>{0.0f, 0.2f, Easing::easeInOutQuad},
            dir
        );
        reveal.setOpen(true);
        m_popupId = GetUIContext().PushPopup(
            Popup(std::move(reveal), UICSpec{})
                .SetAnchor([id = this->id]() {
                    auto* ddl = GetUIContext().FindById<Dropdown<T>>(id);
                    if (!ddl) return Rectangle{};
                    return ddl->GetActualRect();
                })
                .ParentSize(true)
                .Direction(dir)
        );
        m_isOpen = true;
    }

    bool OnUpdate(float dt) override {
        if (auto* reveal = GetReveal()){
            if (reveal->isClosingDone()){
                GetUIContext().RemovePopup(m_popupId);
                m_popupId = 0;
                m_isOpen = false;
            }
        }
        if (m_text.GetText() != m_items[m_currentIdx].first)
            m_text.SetText(m_items[m_currentIdx].first);
        return Button::OnUpdate(dt);
    }
    void Toggle(){
        m_isOpen ? closePopup() : openPopup();
    }
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
                    [id = this->id, i]() {
                        auto* ddl = GetUIContext().FindById<Dropdown<T>>(id);
                        if (!ddl) return;
                        ddl->UpdateSelection(i);
                        if (ddl->m_onChange) ddl->m_onChange(ddl->m_items[i].second);
                        ddl->closePopup();
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
};
