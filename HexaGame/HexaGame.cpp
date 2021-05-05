#include "HexaGame.h"


#include "Entities/Arrow.h"
#include "Entities/DebugPlayer.h"
#include "Entities/DemoMeshEntity.h"

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

    const auto entity = MakeShared<DemoMeshEntity>();
    world->spawn_entity(entity, Vector3(0.0f, 0.0f, 0.0f));

    const auto arrow = MakeShared<Arrow>();
    world->spawn_entity(arrow, Vector3(0.0f, 0.0f, 2.0f));
}
