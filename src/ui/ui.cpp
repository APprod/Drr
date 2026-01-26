#include "debug.hpp"
#include <exception>
#include "ui/ui.hpp"
#include "scenes/layer.hpp"

MyUI::MyUI()
{
}

Button &MyUI::getButton(std::string name)
{
    if (m_buttonMap.count(name) == 0) throw std::invalid_argument("this button name isn't mapped");
    auto idx = m_buttonMap[name];
    return m_buttons[idx];
}

void MyUI::addButton(std::string name, Button &button, ILayer *layer)
{
    if (m_buttonMap.find(name) != m_buttonMap.end()) {fwk::D_Pr("this button name is already mapped"); return;}
    m_buttons.push_back(button);
    m_buttonMap[name] = m_buttons.size() - 1;

    layer->addButton(name);
}