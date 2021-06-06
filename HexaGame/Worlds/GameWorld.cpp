#include "GameWorld.h"

#include "Engine/JSON.h"
#include "HexaGame/HexaGame.h"
#include "HexaGame/HexaMath.h"
#include "HexaGame/Tiles.h"
#include "HexaGame/WorldChunk.h"
#include "HexaGame/Entities/DebugPlayer.h"
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
