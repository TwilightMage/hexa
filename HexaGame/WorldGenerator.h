#pragma once

#include "TileInfo.h"
#include "TileSide.h"
#include "Engine/BasicTypes.h"
#include "Engine/Mesh.h"

class WorldGenerator
{
public:
    static void generate_tile(TileSide sides, const Shared<const TileInfo>& tileInfo, List<Mesh::vertex>& vertices, List<uint>& indices, float seed);
};
