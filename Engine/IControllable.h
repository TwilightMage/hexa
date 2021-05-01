#pragma once

EXTERN class EXPORT IControllable
{
public:
    virtual void key_down(int key) = 0;
    virtual void key_up(int key) = 0;

    virtual void on_possess() {};
    virtual void on_unpossess() {};
};
