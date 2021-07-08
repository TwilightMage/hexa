﻿#include "TileDemoEntity.h"


#include "TileDemoDomainEntity.h"
#include "Engine/GeometryEditor.h"
#include "Engine/World.h"
#include "Engine/Physics/ConcaveMeshCollision.h"
#include "HexaGame/TileIndex.h"
#include "HexaGame/WorldGenerator.h"

TileDemoEntity::TileDemoEntity(const std::array<Shared<const SolidTileInfo>, 6>& tiles)
    : tiles_(tiles)
{
}

void TileDemoEntity::on_start()
{
    if (auto world = get_world())
    {
        std::map<Shared<Texture>, List<Mesh::Vertex>> domains;
        for (uint i = 0; i < 6; i++)
        {
            if (tiles_[i])
            {
                List<Mesh::Vertex> tile_vertices;
                List<uint> tile_indices;
        
                WorldGenerator::generate_tile_mesh(masks[i], tiles_[i], tile_vertices, tile_indices, 0.0f);
                GeometryEditor::remove_indices(tile_vertices, tile_indices);
                GeometryEditor::translate(tile_vertices, tile_positions[i].to_vector());

                domains[tiles_[i]->texture] += tile_vertices;
            }
        }

        for (auto& domain : domains)
        {
            auto domain_mesh = MakeShared<TileDemoDomainEntity>();
            domain_mesh->set_position(get_position());
            domain_mesh->set_rotation(get_rotation());
            domain_mesh->set_scale(get_scale());
            auto mesh = MakeShared<Mesh>("Tile demo mesh", domain.second);
            domain_mesh->set_mesh(mesh);
            domain_mesh->get_material_instance()->set_param_value("texture", domain.first);
            world->spawn_entity(domain_mesh);
            domain_mesh->set_collision(MakeShared<ConcaveMeshCollision>(mesh));

            child_domains_.add(domain_mesh);
        }
    }
}
