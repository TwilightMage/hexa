#include "HexaGame.h"


#include "DebugPlayer.h"
#include "MeshEntity.h"

HexaGame::HexaGame(int argc, char* argv[])
    : Game(argc, argv)
{
}

void HexaGame::init_game_info(GameInfo& outInfo)
{
    outInfo.title = "Hexa";
}

void HexaGame::start()
{
    List<uint> l = {1, 2, 3, 4};
    l.Insert(10, 2);
    
    lock_mouse();
    hide_mouse();
    
    auto world = MakeShared<World>();
    open_world(world);

    const auto player = MakeShared<DebugPlayer>();
    world->spawn_entity(player, Vector3(-3.0f, 0.0f, 0.0f), Quaternion(Vector3(0.0f, 0.0f, 0.0f)));
    possess(player);

    auto mesh = Mesh::load_obj("resources/engine/meshes/icosphere.obj");
    for (auto& vertex : mesh->vertices_)
    {
        vertex.col.x = rand() % 100 / 100.0f;
        vertex.col.y = rand() % 100 / 100.0f;
        vertex.col.z = rand() % 100 / 100.0f;
    }
    const auto sphere = MakeShared<MeshEntity>(mesh);
    world->spawn_entity(sphere, Vector3::zero());
    sphere->scale = Vector3(20.0f);
}
