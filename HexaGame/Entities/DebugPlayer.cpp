#include "DebugPlayer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Arrow.h"
#include "Engine/Camera.h"
#include "Engine/Game.h"
#include "Engine/World.h"

DebugPlayer::DebugPlayer()
    : Entity()
{
}

void DebugPlayer::on_start()
{
    camera_ = MakeShared<Camera>();
    camera_->owner = this;
    camera_->fov = 45.0f;

    if (auto world_ptr = get_world().lock())
    {
        auto arrows_mesh = Mesh::load_obj("resources/engine/meshes/axis_arrows.obj");
        for (auto& vertex : arrows_mesh->vertices_)
        {
            vertex.col = vertex.pos;
        }
        
        arrows_ = MakeShared<MeshEntity>(arrows_mesh);
        arrows_->set_scale(Vector3(0.1f));
        world_ptr->spawn_entity(arrows_);
    }
}

void DebugPlayer::key_down(int key)
{
    if (key == GLFW_KEY_W) move_forward_ += 1;
    else if (key == GLFW_KEY_S) move_forward_ -= 1;
    if (key == GLFW_KEY_D) move_right_ += 1;
    else if (key == GLFW_KEY_A) move_right_ -= 1;
    if (key == GLFW_KEY_SPACE) move_up_ += 1;
    else if (key == GLFW_KEY_LEFT_SHIFT) move_up_ -= 1;
}

void DebugPlayer::key_up(int key)
{
    if (key == GLFW_KEY_W) move_forward_ -= 1;
    else if (key == GLFW_KEY_S) move_forward_ += 1;
    if (key == GLFW_KEY_D) move_right_ -= 1;
    else if (key == GLFW_KEY_A) move_right_ += 1;
    if (key == GLFW_KEY_SPACE) move_up_ -= 1;
    else if (key == GLFW_KEY_LEFT_SHIFT) move_up_ += 1;
}

void DebugPlayer::mouse_button_down(int button)
{
    if (button == 1)
    {
        if (auto world_ptr = get_world().lock())
        {
            const auto arrow = MakeShared<Arrow>();
            world_ptr->spawn_entity(arrow, get_position() + get_rotation().forward(), get_rotation());
        }
    }
}

void DebugPlayer::on_possess()
{
    Game::use_camera(camera_);
}

void DebugPlayer::tick(float delta_time)
{
    auto pos = get_position();
    pos += get_rotation().forward() * delta_time * move_forward_;
    pos += get_rotation().right() * delta_time * move_right_;
    pos.z += delta_time * move_up_;
    set_position(pos);

    auto rot = get_rotation();
    rot = rot.rotate_around_z(Game::get_mouse_delta().x / 10.0f).normalized();
    rot = rot.rotate_around(rot.right(), Game::get_mouse_delta().y / 10.0f).normalized();
    set_rotation(rot);
    
    arrows_->set_position(get_position() + get_rotation().forward());
}
