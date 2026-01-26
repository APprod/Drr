#include "debug.hpp"
#include "core/logic.hpp"
#include "raylib.h"
#include "core/util.hpp"
#include "ui/ui.hpp"

static const bool LDBG = false;
static const bool ALL = false;

LogicController::LogicController()
{
}

LogicController::~LogicController()
{
}

void LogicController::init(GData &data)
{
}

bool LogicController::updateButtons(MyInput &input, MyUI &ui, ILayer *layer)
{
    bool wasClick = false;
    auto &names = layer->getButtonNames();

    bool clicked = input.getClick();
    bool released = input.getRelease();
    bool already_set = false;
    if (releasedButton) // reset previous
    {
        releasedButton->release = false;
        releasedButton = nullptr;
    }
    for (auto &name: names)
    {
        auto &button = ui.getButton(name);
        bool isOver = CheckCollisionPointRec(input.getActivePos(), rect(button));

        fwk::D_Pr("Button: " + name + " is hold:", button.hold, LDBG && button.hold);
        fwk::D_Pr("Button: " + name + " is release:", button.release, LDBG && button.release);
        fwk::D_Pr("Button: " + name + " is click:", button.click, LDBG && button.click);
        button.release = false;

        if (registeredButton && !button.hold) //reset unclicked buttons
        {
            button.hover = false;
        }
        else if (registeredButton && isOver && button.hold)
        {
            button.hover = true;
        }
        else if (!registeredButton && isOver)
        {
            button.hover = true;
        }
        else if (!isOver)
        {
            button.hover = false;
        }
        
        if (isOver && clicked && !already_set) // if that was just clicked
        {
            fwk::D_Pr("Got click on: " + name, LDBG);
            button.hold = true;
            already_set = true;
            wasClick = true;
            registeredButton = true;
        }
        else if (isOver && released && button.hold) // if that was holded
        {
            button.release = true;
            releasedButton = &button;
            button.hold = false;

            registeredButton = false;
        }
        else if (!isOver && released && button.hold) // if that was holded but released somevere else
        {
            button.release = false;
            button.hold = false;

            registeredButton = false;
        }

    }
    return wasClick;
}
