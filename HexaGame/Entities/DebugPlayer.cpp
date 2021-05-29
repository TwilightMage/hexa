﻿#include "DebugPlayer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Arrow.h"
#include "Engine/Camera.h"
#include "Engine/Game.h"
#include "Engine/World.h"
#include "HexaGame/WorldChunkObserver.h"
#include "Engine/ui/TextBlock.h"

DebugPlayer::DebugPlayer()
    : Entity()
{
}

void DebugPlayer::on_start()
{
    camera_ = MakeShared<Camera>();
    camera_->owner = this;
    camera_->fov = 45.0f;

    if (auto world = get_world())
    {
        auto arrows_mesh = Mesh::load_obj("resources/engine/meshes/axis_arrows.obj");
        for (auto& vertex : arrows_mesh->vertices_)
        {
            vertex.col = vertex.pos;
        }
        
        arrows_ = MakeShared<MeshEntity>(arrows_mesh);
        arrows_->set_scale(Vector3(0.1f));
        world->spawn_entity(arrows_);
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
        if (auto world = get_world())
        {
            const auto arrow = MakeShared<Arrow>();
            world->spawn_entity(arrow, get_position() + get_rotation().forward(), get_rotation());
        }
    }
}

void DebugPlayer::on_possess()
{
    Game::use_camera(camera_);
}

void DebugPlayer::tick(float delta_time)
{
    static float speed = 3;
    auto pos = get_position();
    pos += get_rotation().forward() * delta_time * move_forward_ * speed;
    pos += get_rotation().right() * delta_time * move_right_ * speed;
    pos.z += delta_time * move_up_ * speed;
    set_position(pos);

    auto rot = get_rotation();
    rot = rot.rotate_around_z(Game::get_mouse_delta().x / 10.0f).normalized();
    rot = rot.rotate_around(rot.right(), Game::get_mouse_delta().y / 10.0f).normalized();
    set_rotation(rot);
    
    arrows_->set_position(get_position() + get_rotation().forward());

    const ChunkIndex current_chunk = ChunkIndex::from_vector(get_position());

    if (current_chunk != old_chunk_)
    {
        if (observer_)
        {
            observer_->move(current_chunk);
            debug_text_->set_text(StringFormat("Chunk: %i, %i", current_chunk.x, current_chunk.y));
        }
        old_chunk_ = current_chunk;
    }
}

void DebugPlayer::use_observer(const Shared<WorldChunkObserver>& observer)
{
    observer_ = observer;
    
    debug_text_ = MakeShared<TextBlock>(StringFormat("Chunk: %i, %i", observer->get_index().x, observer->get_index().y));
    Game::add_ui(debug_text_);
}
