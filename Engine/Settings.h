#pragma once

#include "framework.h"

EXTERN class EXPORT Settings
{
public:
    virtual ~Settings() {}
    
    FORCEINLINE float get_physics_tick_interval() const { return physics_tick_interval_; }

private:
    float physics_tick_interval_ = 1.0f / 60.0f;
};
