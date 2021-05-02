#include "DebugPlayer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


#include "Engine/Camera.h"
#include "Engine/Game.h"

void DebugPlayer::start()
{
    camera = MakeShared<Camera>();
    camera->owner = this;
    camera->fov = 45.0f;

    if (auto world_ptr = get_world().lock())
    {
        auto arrows_mesh = Mesh::load_obj("resources/engine/meshes/axis_arrows.obj")->remove_indices();
        for (auto& vertex : arrows_mesh->vertices)
        {
            vertex.col = vertex.pos;
        }
        
        arrows_ = MakeShared<MeshEntity>(arrows_mesh);
        arrows_->scale = Vector3(0.1f);
        world_ptr->spawn_entity(arrows_, Vector3::zero());
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

void DebugPlayer::on_possess()
{
    Game::use_camera(camera);
}

void DebugPlayer::tick(float delta_time)
{   
    position += rotation.forward() * delta_time * move_forward_;
    position += rotation.right() * delta_time * move_right_;
    position.z += delta_time * move_up_;

    rotation = rotation.rotate_around_z(Game::get_mouse_delta().x / 10.0f).normalized();
    rotation = rotation.rotate_around(rotation.right(), Game::get_mouse_delta().y / 10.0f).normalized();

    arrows_->position = position + rotation.forward();
}
