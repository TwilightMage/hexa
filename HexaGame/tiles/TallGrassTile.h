#pragma once

#include "HexaGame/TileInfo.h"

class TallGrassTile : public ComplexTileInfo
{
public:
    TallGrassTile(
        const String& key,
        const Shared<Mesh>& mesh,
        const Shared<Texture>& texture,
        const Shared<Material3D>& material
        )
        : ComplexTileInfo(key, mesh, texture, material)
    {}

    void neighbor_changed(const TileIndex& index, TileSide side, const Shared<HexaWorld>& world, const Shared<const TileInfo>& new_neighbor) const override;
    void setup_spawned_entity(const Shared<ComplexTile>& new_entity) const override;
};
