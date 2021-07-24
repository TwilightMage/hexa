﻿#pragma once

#include "TileIndex.h"
#include "Engine/Pointers.h"

class ComplexTileCustomData;
class WorldChunk;
class WorldGenerator;
class TileInfo;

class EXPORT EditableChunk
{
    friend WorldGenerator;
    
public:
    FORCEINLINE const Shared<WorldChunk>& get_chunk() const;
    FORCEINLINE Shared<const TileInfo>& tile(const TileIndex& index) const;
    FORCEINLINE Shared<const TileInfo>& tile(int x, int y, int z) const;

    void set_complex_custom_data(const TileIndex& index, const Shared<ComplexTileCustomData>& custom_data) const;

private:
    explicit EditableChunk(const Shared<WorldChunk>& chunk);
    
    Shared<WorldChunk> chunk_;
};
