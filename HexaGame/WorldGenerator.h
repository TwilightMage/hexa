#pragma once
#include "TileInfo.h"
#include "Engine/BasicTypes.h"
#include "Engine/Mesh.h"

enum class TileSide
{
    Front       = 1,
    FrontRight  = 2,
    BackRight   = 4,
    Back        = 8,
    BackLeft    = 16,
    FrontLeft   = 32,
    Up          = 64,
    Down        = 128
};
IMPLEMENT_ENUM_FLAGS(TileSide);

class WorldGenerator
{
public:
    static void generate_tile(TileSide sides, const Shared<const TileInfo>& tileInfo, List<Mesh::vertex>& vertices, List<uint>& indices);
};
