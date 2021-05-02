#pragma once

EXTERN class EXPORT IControllable
{
public:
    virtual void key_down(int key) {};
    virtual void key_up(int key) {};
    virtual void mouse_button_down(int button) {};
    virtual void mouse_button_up(int button) {};
    
    virtual void on_possess() {};
    virtual void on_unpossess() {};
};
