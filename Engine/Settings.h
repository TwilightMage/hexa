#pragma once

#include "framework.h"

class EXPORT Settings
{
public:
    virtual ~Settings() {}
    
    FORCEINLINE float get_physics_tick_interval() const { return physics_tick_interval_; }
    FORCEINLINE uint get_fps_limit() const { return fps_limit; }

private:
    float physics_tick_interval_ = 1.0f / 60.0f;
    uint fps_limit = 144;
};
