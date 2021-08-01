#include "Player.h"

#include "CameraComponent.h"
#include "Game.h"

Player::Player()
    : Entity()
{
    tick_enabled = true;

    camera_ = create_component<CameraComponent>();
}

void Player::on_possess()
{
    Game::use_camera(camera_);
}
