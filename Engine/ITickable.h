#pragma once

EXTERN class EXPORT ITickable
{
public:
    virtual void tick(float delta_time) = 0;
};
