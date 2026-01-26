/* controls all logic of the App */
#pragma once

#include "core/myinput.hpp"
#include "scenes/layer.hpp"
#include "ui/ui.hpp"

class LogicController
{
public:
    LogicController();
    void init(GData &data);
    ~LogicController();

    bool updateButtons(MyInput &input, MyUI &ui, ILayer *layer);
    // void update(MyInput &input, MyUI &ui, GData &data);
private:
    bool registeredButton = false;
    Button *releasedButton = nullptr;
};
