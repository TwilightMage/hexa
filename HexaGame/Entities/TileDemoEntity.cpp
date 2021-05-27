#include "TileDemoEntity.h"


#include "Engine/GeometryEditor.h"
#include "Engine/World.h"
#include "HexaGame/TileIndex.h"
#include "HexaGame/WorldGenerator.h"

TileDemoEntity::TileDemoEntity(const std::array<Shared<const TileInfo>, 6>& tiles)
    : tiles_(tiles)
{
}

void TileDemoEntity::on_start()
{
    if (auto world = get_world())
    {
        std::map<Shared<Texture>, List<Mesh::vertex>> domains;
        for (uint i = 0; i < 6; i++)
        {
            if (tiles_[i])
            {
                List<Mesh::vertex> tile_vertices;
                List<uint> tile_indices;
        
                WorldGenerator::generate_tile_mesh(masks[i], tiles_[i], tile_vertices, tile_indices, 0.0f);
                GeometryEditor::remove_indices(tile_vertices, tile_indices);
                GeometryEditor::translate(tile_vertices, tile_positions[i].to_vector());

                domains[tiles_[i]->texture] += tile_vertices;
            }
        }

        for (auto& domain : domains)
        {
            auto domain_mesh = MakeShared<Entity>();
            domain_mesh->set_position(get_position());
            domain_mesh->set_rotation(get_rotation());
            domain_mesh->set_scale(get_scale());
            domain_mesh->use_mesh(MakeShared<Mesh>("Tile demo mesh", domain.second));
            domain_mesh->use_texture(domain.first);
            world->spawn_entity(domain_mesh);

            child_domains_.Add(domain_mesh);
        }
    }
}
