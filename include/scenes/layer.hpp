#pragma once
#include <string>
#include <unordered_set>
#include <string>
#include <vector>

using BNames = std::unordered_set<std::string>;


class ILayer
{
public:
    virtual ~ILayer() = default;

    
    void addButton(std::string name);  
    BNames& getButtonNames() {return m_buttons;}  

    bool isActive = false;
private:
    BNames m_buttons;
};

using Layers=std::vector<ILayer *>;