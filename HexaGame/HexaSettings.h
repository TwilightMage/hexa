#pragma once

#include "Engine/Settings.h"

class EXPORT HexaSettings : public Settings
{
public:
    float audio_music = 1.0f;
    float audio_ambient = 1.0f;
    float audio_effects = 1.0f;

    uint chunk_load_distance = 9;
    
    void read_settings(const Compound::Object& json) override;
    Compound::Object write_settings() override;
};
