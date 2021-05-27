#pragma once

#include "TileIndex.h"
#include "Engine/BasicTypes.h"

class WorldGenerator;
class TileInfo;
class WorldChunkData;

EXTERN class EXPORT EditableChunk
{
    friend WorldGenerator;
public:
    const Shared<WorldChunkData>& get_chunk() const;
    Shared<const TileInfo>& tile(const TileIndex& index) const;

private:
    explicit EditableChunk(const Shared<WorldChunkData>& chunk);
    
    Shared<WorldChunkData> chunk_;
};
