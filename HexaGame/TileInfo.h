#pragma once

#include "TileType.h"
#include "Engine/DatabaseEntry.h"

class Texture;

class TileInfo : public DatabaseEntry
{
public:
    TileType type;
    
    bool randomize_ceil_uv_angle;
    bool randomize_floor_uv_angle;

    Shared<Texture> texture;

    String drop_item_name;
};
