#pragma once

#include "HexaGame/TileInfo.h"

class GrassTile : public SolidTileInfo
{
public:
    GrassTile(
        const String& key,
        const Shared<Texture>& texture,
        bool randomize_ceil_uv_angle,
        bool randomize_floor_uv_angle
        )
        : SolidTileInfo(key, texture, randomize_ceil_uv_angle, randomize_floor_uv_angle)
    {}

    List<ItemContainer> get_drops(const TileIndex& index, const Shared<HexaWorld>& world) const override;
};
