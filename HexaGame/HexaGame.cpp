#include "HexaGame.h"


#include "DebugPlayer.h"
#include "DemoMeshEntity.h"

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
    auto world = MakeShared<World>();
    open_world(world);

    auto player = MakeShared<DebugPlayer>();
    world->spawn_entity(player, glm::vec3(-5.0f, 0.0f, 3.0f), glm::quat(glm::vec3(0.0f, glm::radians(30.0f), 0.0f)));
    possess(player);

    world->spawn_entity(MakeShared<DemoMeshEntity>(), glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));
}
