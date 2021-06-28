#pragma once

#include "TileType.h"
#include "Engine/DatabaseEntry.h"

class Texture;

class TileInfo : public DatabaseEntry
{
public:
    TileInfo(
        const String& key,
        const Shared<Texture>& texture,
        TileType type,
        bool randomize_ceil_uv_angle,
        bool randomize_floor_uv_angle
        )
        : DatabaseEntry(key)
        , texture(texture)
        , type(type)
        , randomize_ceil_uv_angle(randomize_ceil_uv_angle)
        , randomize_floor_uv_angle(randomize_floor_uv_angle)
    {}
    
    Shared<Texture> texture;
    
    TileType type;
    
    bool randomize_ceil_uv_angle;
    bool randomize_floor_uv_angle;
};
