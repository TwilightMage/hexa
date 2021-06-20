#pragma once

#include "UIElement.h"

class UIInputElement : public UIElement
{
public:
    virtual void on_focus() {};
    virtual void on_unfocus() {};
    
    virtual void key_down(int key) {}
    virtual void key_up(int key) {}
    virtual void key_hold(int key) {}

    virtual void text_input(uint code) {}
};
