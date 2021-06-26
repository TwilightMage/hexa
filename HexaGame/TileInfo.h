#pragma once

#include "TileType.h"
#include "Engine/DatabaseEntry.h"

class Texture;

class TileInfo : public DatabaseEntry
{
public:
    Shared<Texture> texture;
    
    TileType type;
    
    bool randomize_ceil_uv_angle;
    bool randomize_floor_uv_angle;

    String drop_item_name;
};
