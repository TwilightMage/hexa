#pragma once

#include "Vector2.h"

class EXPORT IControllable
{
public:
    virtual void key_down(int key) {};
    virtual void key_up(int key) {};
    virtual void mouse_button_down(int button) {};
    virtual void mouse_button_up(int button) {};
    virtual void scroll(const Vector2& delta) {};
    
    virtual void on_possess() {};
    virtual void on_unpossess() {};
};
