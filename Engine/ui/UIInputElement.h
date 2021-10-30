#pragma once

#include "UIElement.h"
#include "Engine/KeyCode.h"

class UIInputElement : public UIElement
{
public:
    virtual void on_focus() {};
    virtual void on_unfocus() {};
    
    virtual void key_down(KeyCode key) {}
    virtual void key_up(KeyCode key) {}
    virtual void key_hold(KeyCode key) {}

    virtual void text_input(uint code) {}
};
