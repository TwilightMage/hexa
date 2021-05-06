#include "CollisionEditorPlayer.h"


#include "Engine/Camera.h"
#include "Engine/Game.h"
#include "HexaGame/Entities/MeshEntity.h"

void CollisionEditorPlayer::on_start()
{
    camera_ = MakeShared<Camera>();
    camera_->owner = this;
    camera_->fov = 45.0f;
}

void CollisionEditorPlayer::tick(float delta_time)
{
    if (sample)
    {
        if (is_rotating_)
        {
            auto rot = sample->get_rotation();
            rot = rot.rotate_around_z(Game::get_mouse_delta().x * -0.2f);
            rot = rot.rotate_around_y(Game::get_mouse_delta().y * -0.2f);
            sample->set_rotation(rot);
        }
    }
}

void CollisionEditorPlayer::on_possess()
{
    Game::use_camera(camera_);
}

void CollisionEditorPlayer::mouse_button_down(int button)
{
    if (button == 0)
    {
        is_rotating_ = true;
    }
}

void CollisionEditorPlayer::mouse_button_up(int button)
{
    if (button == 0)
    {
        is_rotating_ = false;
    }
}
