#pragma once

#include "TileInfo.h"
#include "TileSide.h"
#include "Engine/BasicTypes.h"
#include "Engine/Mesh.h"

class WorldChunkData;

class WorldGenerator
{
public:
    static void generate_tile_mesh(TileSide sides, const Shared<const TileInfo>& tileInfo, List<Mesh::vertex>& vertices, List<uint>& indices, float seed);

    virtual Shared<WorldChunkData> generate_chunk(int x, int y) = 0;
};
