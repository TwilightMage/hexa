#include "Player.h"

#include "Camera.h"

void Player::on_possess()
{
    Game::use_camera(cast<ICamera>(shared_from_this()));
}

CameraInfo Player::get_camera_info() const
{
    return {
        get_position(),
        get_rotation()
    };
}
