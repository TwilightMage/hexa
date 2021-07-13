#pragma once

#include "framework.h"
#include "JSON.h"

class EXPORT Settings
{
public:
    virtual ~Settings() {}
    
    FORCEINLINE float get_physics_tick_interval() const { return physics_tick_interval_; }

    uint fps_limit = 60;

    float audio_general = 1.0f;

    virtual void read_settings(const JSON& json);
    virtual void write_settings(JSON& json);

private:
    float physics_tick_interval_ = 1.0f / 60.0f;
};
