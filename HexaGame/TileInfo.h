#pragma once

#include "ItemContainer.h"
#include "TileIndex.h"
#include "TileType.h"
#include "Engine/DatabaseEntry.h"

class World;
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

    virtual void neighbor_changed(const TileIndex& index, TileSide side, const Shared<World>& world, const Shared<const TileInfo>& new_neighbor) const;
    virtual List<ItemContainer> get_drops(const TileIndex& index, const Shared<World>& world) const;
};
