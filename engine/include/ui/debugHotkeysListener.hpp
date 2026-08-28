#pragma once
#include "ui/hotkeysListener.hpp"

class DebugHotkeysListener: public HotkeysListener{
    bool OnEvent(const MyEvent& event) override;
};