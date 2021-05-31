#pragma once

#include "WorldGenerator.h"
#include "Engine/PerlinNoise.h"

EXTERN class EXPORT DefaultWorldGenerator : public WorldGenerator
{
public:
    void init(uint seed) override;
    
    mINI::INIStructure write_settings() const override;
    void read_settings(const mINI::INIStructure& settings) override;

protected:
    void generate_chunk(const EditableChunk& editable) override;

private:
    float ground_level = 30.0f;
    float ground_amplitude = 10.0f;

    siv::PerlinNoise generator_;
};
