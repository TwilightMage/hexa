#pragma once

#include "HexaGame/TileInfo.h"

class GrassInfo : public SolidTileInfo
{
public:
    GrassInfo(
        const String& key,
        const Set<String>& tags,
        float hardness,
        const Shared<Texture>& texture,
        bool randomize_ceil_uv_angle,
        bool randomize_floor_uv_angle
        )
        : SolidTileInfo(key, tags + Set{ MetaTags::PLANT, MetaTags::GRASS }, hardness, texture, randomize_ceil_uv_angle, randomize_floor_uv_angle)
    {}

    List<ItemContainer> get_drops(const TileIndex& index, const Shared<HexaWorld>& world) const override;
};
