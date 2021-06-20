#pragma once

#include "WorldGenerator.h"
#include "Engine/PerlinNoise.h"

class EXPORT DefaultWorldGenerator : public WorldGenerator
{
public:
    void init(uint seed) override;
    
    JSON write_settings() const override;
    void read_settings(const JSON& settings) override;

protected:
    void generate_chunk(const EditableChunk& editable) override;

private:
    float ground_level = 30.0f;
    float ground_amplitude = 10.0f;

    siv::PerlinNoise generator_;
};
