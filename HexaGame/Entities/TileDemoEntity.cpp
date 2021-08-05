#include "TileDemoEntity.h"

#include "Engine/GeometryEditor.h"
#include "Engine/MeshComponent.h"
#include "Engine/World.h"
#include "Engine/Physics/ConcaveMeshCollision.h"
#include "HexaGame/TileIndex.h"
#include "HexaGame/WorldGenerator.h"

TileDemoEntity::TileDemoEntity(const std::array<ConstPtr<SolidTileInfo>, 6>& tiles)
    : tiles_(tiles)
{
    mesh_component_ = create_component<MeshComponent>();
}

void TileDemoEntity::on_start()
{
    if (auto world = get_world())
    {
        SimpleMap<ConstPtr<SolidTileInfo>, StaticMesh::SubMesh> sub_meshes;
        for (uint i = 0; i < 6; i++)
        {
            if (tiles_[i])
            {
                List<StaticMesh::Vertex> tile_vertices;
                List<uint> tile_indices;
        
                WorldGenerator::generate_tile_mesh(masks[i], tiles_[i], tile_vertices, tile_indices, 0.0f);
                GeometryEditor::translate(tile_vertices, tile_positions[i].to_vector());

                sub_meshes[tiles_[i]].add(tile_vertices, tile_indices);
            }
        }

        Vector3 offset;
        uint c = 0;
        for (auto& sub_mesh : sub_meshes)
        {
            for (auto& vertex : sub_mesh->value.vertices)
            {
                offset.x += vertex.pos.x;
                offset.z += vertex.pos.z;
                c++;
            }
        }

        offset /= (float)c;

        for (auto& sub_mesh : sub_meshes)
        {
            GeometryEditor::translate(sub_mesh->value.vertices, -offset);
        }

        mesh_component_->set_mesh(StaticMesh::construct("tile demo", sub_meshes.get_values(), AutoCollisionMode::Complex));
        for (uint i = 0; i < sub_meshes.size(); i++)
        {
            mesh_component_->set_material(sub_meshes.entries[i]->key->material, i);
        }
        mesh_component_->set_body_type(PhysicalBodyType::Kinematic);
    }
}
