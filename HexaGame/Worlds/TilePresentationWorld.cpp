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

            Shared<StaticMesh> mesh = StaticMesh::construct(String::format("tile %s", entry.value->key.to_string().c()), { StaticMesh::SubMesh(vertices, indices) }, AutoCollisionMode::Convex);
        
            const auto entity = spawn_entity<SingleTile>(pos);
            entity->mesh()->set_mesh(mesh);
            entity->mesh()->set_material(entry.value->material, 0);
            entity->mesh()->set_body_type(PhysicalBodyType::Kinematic);

            if (i++ == 3)
            {
                x++;
                y--;
                i -= 3;
            }
        }
    }

    spawn_entity<TileDemoEntity>(Vector3(0.0f, 0.0f, -300.0f), std::array<ConstPtr<SolidTileInfo>, 6>{Tiles::dirt, Tiles::dirt, Tiles::grass, Tiles::dirt, Tiles::grass, Tiles::grass});

    set_ambient_light(Color::white(), 0.45f);
    set_directional_light(Color::white(), 0.55f, Quaternion(Vector3(0, 45, 45)));

    const auto player = MakeShared<DebugPlayer>();
    spawn_entity(player, Vector3(0.0f, 50.0f, 300.0f));
    Game::possess(player);

    const auto monkey = spawn_entity<Entity>(Vector3(300.0f, 0.0f, -300.0f));
    auto monkey_mesh = monkey->create_component<MeshComponent>();
    monkey_mesh->set_mesh(StaticMesh::load_file_obj("resources/meshes/monkey.obj", AutoCollisionMode::Complex));
    monkey_mesh->set_material(Tiles::dirt->material, 0);
    monkey_mesh->set_body_type(PhysicalBodyType::Kinematic);

    const auto multi_coll = spawn_entity<Entity>(Vector3(600.0f, 0.0f, -300.0f));
    auto multi_coll_mesh = multi_coll->create_component<MeshComponent>();
    multi_coll_mesh->set_mesh(StaticMesh::load_file_obj("resources/meshes/multi_coll.obj"));
    multi_coll_mesh->set_material(Tiles::dirt->material, 0);
    multi_coll_mesh->set_body_type(PhysicalBodyType::Kinematic);
}
