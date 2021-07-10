#pragma once

#include "Engine/Quaternion.h"
#include "HexaGame/TileInfo.h"

class TreeStemInfo : public ComplexTileInfo
{
public:
    TreeStemInfo(
        const String& key,
        const Shared<Mesh>& mesh,
        const Shared<Texture>& texture,
        const Shared<Material3D>& material,
        const List<Shared<Mesh>>& roots_meshes,
        const List<Shared<Texture>>& roots_textures,
        const Shared<Mesh>& krone_mesh,
        const Shared<Texture>& krone_texture
        )
        : ComplexTileInfo(key, mesh, texture, material)
        , roots_meshes(roots_meshes)
        , roots_textures(roots_textures)
        , krone_mesh(krone_mesh)
        , krone_texture(krone_texture)
    {}

    Shared<ComplexTileCustomData> create_custom_data() const override;
    void setup_spawned_entity(const Shared<ComplexTile>& new_tile, const Shared<ComplexTileCustomData>& custom_data) const override;

    List<Shared<Mesh>> roots_meshes;
    List<Shared<Texture>> roots_textures;
    Shared<Mesh> krone_mesh;
    Shared<Texture> krone_texture;
};
