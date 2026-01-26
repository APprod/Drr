#include "scenes/layer.hpp"
#include <stdexcept>

void ILayer::addButton(std::string name)
{
    if (m_buttons.count(name) != 0) throw std::invalid_argument("this button name is already stated in page");
    m_buttons.insert(name);
}
