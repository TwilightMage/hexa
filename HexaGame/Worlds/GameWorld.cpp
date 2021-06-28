#include "GameWorld.h"

#include "HexaGame/Entities/GamePlayer.h"

GameWorld::GameWorld(const Shared<WorldGenerator>& generator)
    : HexaWorld(generator)
{
}

void GameWorld::on_start()
{
    player_ = MakeShared<GamePlayer>();
    spawn_entity(player_);
}
