#include "GameWorld.h"

#include "HexaGame/HexaGame.h"
#include "HexaGame/Entities/GamePlayer.h"
#include "HexaGame/Entities/Skybox.h"

GameWorld::GameWorld(const Shared<WorldGenerator>& generator)
    : HexaWorld(generator)
{
}

void GameWorld::on_start()
{
    player_ = MakeShared<GamePlayer>();
    spawn_entity(player_);

    skybox_ = MakeShared<Skybox>();
    spawn_entity(skybox_);
}

void GameWorld::on_tick(float delta_time)
{
    skybox_->set_position(Game::get_camera_info().position);
}
