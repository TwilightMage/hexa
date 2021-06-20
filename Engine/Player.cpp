#include "Player.h"

#include "Camera.h"

void Player::on_start()
{
    camera_ = MakeShared<Camera>();
    camera_->owner = this;
    camera_->fov = 45.0f;
}

void Player::on_possess()
{
    Game::use_camera(camera_);
}
