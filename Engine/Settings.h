#pragma once

#include "framework.h"

class EXPORT Settings
{
public:
    virtual ~Settings() {}
    
    FORCEINLINE float get_physics_tick_interval() const { return physics_tick_interval_; }

    uint fps_limit = 144;

private:
    float physics_tick_interval_ = 1.0f / 60.0f;
};
