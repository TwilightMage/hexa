#pragma once

#include "HexaGame/TileInfo.h"

class TallGrassInfo : public ComplexTileInfo
{
public:
    TallGrassInfo(
        const String& key,
        const Set<Name>& tags,
        float hardness,
        const Shared<Mesh>& mesh,
        const Shared<Texture>& texture,
        const Shared<Material3D>& material
        )
        : ComplexTileInfo(key, tags + Set{ MetaTags::PLANT, MetaTags::GRASS }, hardness, mesh, texture, material)
    {}

    void neighbor_changed(const TileIndex& index, TileSide side, const Shared<HexaWorld>& world, const Shared<const TileInfo>& new_neighbor) const override;
    void setup_spawned_entity(const Shared<ComplexTile>& new_entity, const Shared<ComplexTileCustomData>& custom_data) const override;
};
