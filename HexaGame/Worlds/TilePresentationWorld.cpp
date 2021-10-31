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

    set_ambient_light(Color::white(), 0.3f);
    set_directional_light(Color::white(), 0.7f, Quaternion(Vector3(0, 45, 45)));

    const auto player = MakeShared<DebugPlayer>();
    spawn_entity(player, Vector3(-300.0f, 0.0f, 50.0f));
    Game::possess(player);

    spawn_entity<TileDemoEntity>(Transform(Vector3(300.0f, 0.0f, 0.0f)), std::array<ConstPtr<SolidTileInfo>, 6>{Tiles::dirt, Tiles::dirt, Tiles::grass, Tiles::dirt, Tiles::grass, Tiles::grass});

    {
        const auto entity = spawn_entity<Entity>(Transform(Vector3(300.0f, 300.0f, 0.0f), Quaternion(Vector3(0, 0, 180))));
        auto mesh = entity->create_component<MeshComponent>();
        mesh->set_mesh(StaticMesh::load_file_obj("resources/meshes/monkey.obj", AutoCollisionMode::Complex));
        mesh->set_material(HexaGame::get_instance()->get_material("Hexa/UV_Test"), 0);
        mesh->set_body_type(PhysicalBodyType::Kinematic);
    }

    {
        const auto entity = spawn_entity<Entity>(Transform(Vector3(300.0f, 600.0f, 0.0f), Quaternion(Vector3(0, 0, 180))));
        auto mesh = entity->create_component<MeshComponent>();
        mesh->set_mesh(StaticMesh::load_file_obj("resources/meshes/multi_coll_sphere.obj"));
        mesh->set_material(HexaGame::get_instance()->get_material("Hexa/UV_Test"), 0);
        mesh->set_body_type(PhysicalBodyType::Kinematic);
    }

    {
        const auto entity = spawn_entity<Entity>(Transform(Vector3(300.0f, 900.0f, 0.0f), Quaternion(Vector3(0, 0, 180))));
        auto mesh = entity->create_component<MeshComponent>();
        mesh->set_mesh(StaticMesh::load_file_obj("resources/meshes/multi_coll_convex.obj"));
        mesh->set_material(HexaGame::get_instance()->get_material("Hexa/UV_Test"), 0);
        mesh->set_body_type(PhysicalBodyType::Kinematic);
    }

    {
        const auto entity = spawn_entity<Entity>(Transform(Vector3(0.0f, 0.0f, -300.0f)));
        auto mesh = entity->create_component<MeshComponent>();
        mesh->set_mesh(StaticMesh::load_file_obj("resources/meshes/floor.obj", AutoCollisionMode::Convex));
        mesh->set_material(HexaGame::get_instance()->get_material("Hexa/UV_Test"), 0);
        mesh->set_body_type(PhysicalBodyType::Kinematic);
    }
}
