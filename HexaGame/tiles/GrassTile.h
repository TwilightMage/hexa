#pragma once

#include "HexaGame/TileInfo.h"

class GrassTile : public TileInfo
{
public:
    GrassTile(
        const String& key,
        const Shared<Texture>& texture,
        TileType type,
        bool randomize_ceil_uv_angle,
        bool randomize_floor_uv_angle
        )
        : TileInfo(key, texture, type, randomize_ceil_uv_angle, randomize_floor_uv_angle)
    {}

    List<ItemContainer> get_drops(const TileIndex& index, const Shared<World>& world) const override;
};
