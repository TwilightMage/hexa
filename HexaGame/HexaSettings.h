#pragma once

#include "Engine/Settings.h"

class EXPORT HexaSettings : public Settings
{
public:
    float audio_music = 1.0f;
    float audio_ambient = 1.0f;
    float audio_effects = 1.0f;
    
    void read_settings(const JSON& json) override;
    void write_settings(JSON& json) override;
};
