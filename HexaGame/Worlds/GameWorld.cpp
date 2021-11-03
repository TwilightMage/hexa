#include "GameWorld.h"

#include "HexaGame/HexaGame.h"
#include "HexaGame/Entities/GamePlayer.h"
#include "HexaGame/Entities/DebugPlayer.h"

GameWorld::GameWorld(const Shared<WorldGenerator>& generator)
    : HexaWorld(generator)
{
}

void GameWorld::on_start()
{
    HexaWorld::on_start();

    if (Utils::ask_yes_no("Start as debug player?"))
    {
        player_ = MakeShared<DebugPlayer>();
    }
    else
    {
        player_ = MakeShared<GamePlayer>();
    }
    spawn_entity(player_);
    Game::possess(player_);
}
