#include "TilePresentationWorld.h"

#include "Engine/GeometryEditor.h"
#include "Engine/MeshComponent.h"
#include "Engine/Physics/ConcaveMeshCollision.h"
#include "Engine/Physics/ConvexMeshCollision.h"
#include "HexaGame/HexaGame.h"
#include "HexaGame/WorldGenerator.h"
#include "HexaGame/Database/Tiles.h"
#include "HexaGame/Entities/DebugPlayer.h"
#include "HexaGame/Entities/SingleTile.h"
#include "HexaGame/Entities/TileDemoEntity.h"

void TilePresentationWorld::on_start()
{
    uint x = 0;
    uint y = 0;
    uint i = 2;
    for (auto& entry : HexaGame::tile_database->records())
    {
        if (auto solid_entry = cast<SolidTileInfo>(entry.value))
        {
            const Vector3 pos = TileIndex(x, y++, 0).to_vector();
        
            List<StaticMesh::Vertex> vertices;
            List<uint> indices;
            WorldGenerator::generate_tile_mesh(TileSide::All, solid_entry, vertices, indices, pos.x + pos.y + pos.z);
            GeometryEditor::remove_indices(vertices, indices);

            Shared<StaticMesh> mesh = StaticMesh::construct(String::format("tile %s", entry.value->key.to_string().c()), { StaticMesh::SubMesh(vertices, indices) }, AutoCollisionMode::Convex);
        
            const auto entity = spawn_entity<SingleTile>(pos);
            entity->mesh()->set_mesh(GeometryEditor::get_unit_cube());
            entity->mesh()->set_material(entry.value->material, 0);
            //entity->set_collision(MakeShared<ConvexMeshCollision>(mesh));
            //entity->get_material_instance()->set_param_value("texture", solid_entry->texture);

            if (i++ == 3)
            {
                x++;
                y--;
                i -= 3;
            }
        }
    }

    spawn_entity<TileDemoEntity>(TileIndex(-3, 0, 0).to_vector(), std::array<Shared<const SolidTileInfo>, 6>{Tiles::dirt, Tiles::dirt, Tiles::grass, Tiles::dirt, Tiles::grass, Tiles::grass});

    set_ambient_light(Color::white(), 0.45f);
    set_directional_light(Color::white(), 0.55f, Quaternion(Vector3(0, 45, 45)));

    const auto player = MakeShared<DebugPlayer>();
    spawn_entity(player, Vector3(0.0f, 50.0f, 300.0f));
    Game::possess(player);

    const auto monkey_head = spawn_entity<Entity>(TileIndex(-3, 3, 0).to_vector());
    auto monkey_mesh = monkey_head->create_component<MeshComponent>();
    monkey_mesh->set_mesh(/*StaticMesh::load_file_obj("resources/meshes/monkey.obj")*/GeometryEditor::get_unit_cube());
    monkey_mesh->set_material(Tiles::dirt->material, 0);
}
