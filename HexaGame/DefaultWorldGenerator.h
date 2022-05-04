#pragma once

#include "WorldGenerator.h"
#include "Engine/Compound.h"
#include "Engine/PerlinNoise.h"

class EXPORT DefaultWorldGenerator : public WorldGenerator
{
public:
    void init(uint seed) override;
    
    Compound::Object write_settings() const override;
    void read_settings(const Compound::Object& settings) override;

protected:
    void generate_chunk(const EditableChunk& editable) override;

private:
    void generate_plains(const EditableChunk& editable);
    void generate_forest(const EditableChunk& editable);
    void generate_debug(const EditableChunk& editable);

    FORCEINLINE void generate_land(const EditableChunk& editable) const;
    
    float ground_level = 30.0f;
    float ground_amplitude = 10.0f;

    siv::PerlinNoise generator_;
};
