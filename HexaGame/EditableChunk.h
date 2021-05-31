#pragma once

#include "TileIndex.h"
#include "Engine/BasicTypes.h"

class WorldChunk;
class WorldGenerator;
class TileInfo;

EXTERN class EXPORT EditableChunk
{
    friend WorldGenerator;
    
public:
    const Shared<WorldChunk>& get_chunk() const;
    Shared<const TileInfo>& tile(const TileIndex& index) const;

private:
    explicit EditableChunk(const Shared<WorldChunk>& chunk);
    
    Shared<WorldChunk> chunk_;
};
