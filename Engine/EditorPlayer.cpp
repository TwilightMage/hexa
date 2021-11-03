#include "EditorPlayer.h"

#include "Game.h"

void EditorPlayer::on_tick(float delta_time)
{
    distance_ = Math::lerp(distance_, target_distance_, delta_time * 10);
    
    if (is_rotating_)
    {
        auto rot = get_rotation();
        rot = rot.rotate_around_up(Game::get_mouse_delta().x * 0.2f);
        rot = rot.rotate_around(rot.right(), Game::get_mouse_delta().y * 0.2f);
        set_rotation(rot);
    }

    set_location(get_rotation().forward() * -distance_ + get_rotation().rotate_vector3(position_offset));
}

void EditorPlayer::mouse_button_down(int button)
{
    if (button == 0)
    {
        is_rotating_ = true;
    }
}

void EditorPlayer::mouse_button_up(int button)
{
    if (button == 0)
    {
        is_rotating_ = false;
    }
}

void EditorPlayer::scroll(float y)
{
    target_distance_ = Math::clamp(target_distance_ - y, 2.0f, 10.0f);
}