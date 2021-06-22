#include "DebugPlayer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Arrow.h"
#include "Engine/Camera.h"
#include "Engine/Game.h"
#include "Engine/World.h"
#include "Engine/Physics/RaycastResult.h"
#include "Engine/ui/TextBlock.h"
#include "HexaGame/Paths.h"
#include "HexaGame/WorldChunkObserver.h"
#include "HexaGame/Worlds/HexaWorld.h"

void DebugPlayer::on_start()
{
    camera_ = MakeShared<Camera>();
    camera_->owner = this;
    camera_->fov = 45.0f;

    debug_text_ = MakeShared<TextBlock>("qwerty");
    debug_text_->set_position(Vector2(5, 5));
    Game::add_ui(debug_text_);
    
    if (auto world = get_world())
    {
        const auto arrows_mesh = Mesh::load_obj(RESOURCES_HEXA_MESHES + "axis_arrows.obj");
        auto arrows_vertices = arrows_mesh->get_vertices();
        auto arrows_indices = arrows_mesh->get_indices();
        for (auto& vertex : arrows_vertices)
        {
            vertex.col = vertex.pos;
        }
        
        arrows_ = MakeShared<MeshEntity>(MakeShared<Mesh>("Axis Arrows", arrows_vertices, arrows_indices));
        arrows_->set_scale(Vector3(0.1f));
        world->spawn_entity(arrows_);

        if (auto hexa_world = cast<HexaWorld>(world))
        {
            const auto current_chunk = ChunkIndex::from_vector(get_position());
            
            observer_ = hexa_world->register_chunk_observer(current_chunk, load_distance_);
            observer_->set_render_chunks(true);
        }
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
    Game::lock_mouse();
    Game::hide_mouse();
    
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

    const ChunkIndex current_chunk = ChunkIndex::from_vector(get_position());

    if (current_chunk != old_chunk_)
    {
        if (observer_)
        {
            observer_->move(current_chunk, load_distance_);
            debug_text_->set_text(String::format("Chunk: %i, %i", current_chunk.x, current_chunk.y));
        }
        old_chunk_ = current_chunk;
    }

    if (auto world = get_world())
    {
        if (auto hit = world->raycast(get_position(), get_position() + get_rotation().forward() * 10))
        {
            arrows_->set_position(hit->location);
        }
        else
        {
            arrows_->set_position(get_position() + get_rotation().forward());
        }
    }
}
