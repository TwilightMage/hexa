#pragma once

#include "Engine/Quaternion.h"
#include "HexaGame/TileInfo.h"

class TreeStemInfo : public ComplexTileInfo
{
public:
    TreeStemInfo(
        const Name& key,
        const Set<Name>& tags,
        float hardness,
        const Shared<Mesh>& mesh,
        const Shared<Texture>& texture,
        const Shared<Material3D>& material,
        const Shared<Mesh>& roots_mesh,
        const Shared<Texture>& roots_texture,
        const Shared<Mesh>& krone_mesh,
        const Shared<Texture>& krone_texture,
        const Shared<Mesh>& branch_mesh,
        const Shared<Texture>& branch_texture,
        const Shared<Mesh>& branch_krone_mesh,
        const Shared<Texture>& branch_krone_texture,
        const Name& log_item_name
        )
        : ComplexTileInfo(key, tags + Set{ MetaTags::PLANT, MetaTags::WOOD, MetaTags::STEM }, hardness, mesh, texture, material)
        , roots_mesh(roots_mesh)
        , roots_texture(roots_texture)
        , krone_mesh(krone_mesh)
        , krone_texture(krone_texture)
        , branch_mesh(branch_mesh)
        , branch_texture(branch_texture)
        , branch_krone_mesh(branch_krone_mesh)
        , branch_krone_texture(branch_krone_texture)
        , log_item_name(log_item_name)
    {}

    Shared<ComplexTileCustomData> create_custom_data() const override;
    void setup_spawned_entity(const Shared<ComplexTile>& new_tile, const Shared<ComplexTileCustomData>& custom_data) const override;
    void on_tile_destroyed(const TileIndex& index, const Shared<ComplexTile>& destroyed_tile, const Shared<HexaWorld>& world) const override;
    void neighbor_changed(const TileIndex& index, TileSide side, const Shared<HexaWorld>& world, const Shared<const TileInfo>& new_neighbor) const override;

    Shared<Mesh> roots_mesh;
    Shared<Texture> roots_texture;
    Shared<Mesh> krone_mesh;
    Shared<Texture> krone_texture;
    Shared<Mesh> branch_mesh;
    Shared<Texture> branch_texture;
    Shared<Mesh> branch_krone_mesh;
    Shared<Texture> branch_krone_texture;
    Name log_item_name;
};
