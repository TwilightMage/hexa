#include "DebugPlayer.h"

#include "Engine/Game.h"
#include "Engine/Material.h"
#include "Engine/MeshComponent.h"
#include "Engine/World.h"
#include "Engine/Physics/RaycastResult.h"
#include "HexaGame/Paths.h"
#include "HexaGame/WorldChunkObserver.h"
#include "HexaGame/Worlds/HexaWorld.h"

void DebugPlayer::on_start()
{
    Player::on_start();

    pitch_ = get_rotation().pitch();
    yaw_ = get_rotation().yaw();
    
    if (auto world = get_world())
    {
        auto arrows_mat = Game::get_instance()->load_material("Hexa/Axis_Arrows");
        if (!arrows_mat)
        {
            arrows_mat = Game::get_instance()->load_material("Engine/Basic")->clone("Hexa/Axis_Arrows");
            arrows_mat->set_texture("axis_arrows.png", 0);
        }
        
        arrows_ = world->spawn_entity<Entity>(Transform(Vector3(), Quaternion(), Vector3(0.1f)));
        arrows_->create_component<MeshComponent>(StaticMesh::load_file_obj(RESOURCES_MESHES + "axis_arrows.obj"), arrows_mat);

        if (auto hexa_world = cast<HexaWorld>(world))
        {
            const auto current_chunk = ChunkIndex::from_vector(get_location());
            
            observer_ = hexa_world->register_chunk_observer(current_chunk, load_distance_);
            observer_->set_render_chunks(true);
        }
    }
}

void DebugPlayer::key_down(KeyCode key)
{
    Player::key_down(key);

    switch (key)
    {
    case KeyCode::W:
        move_forward_ += 1;
        break;
    case KeyCode::S:
        move_forward_ -= 1;
        break;
    case KeyCode::D:
        move_right_ += 1;
        break;
    case KeyCode::A:
        move_right_ -= 1;
        break;
    case KeyCode::Space:
        move_up_ += 1;
        break;
    case KeyCode::LeftShift:
        move_up_ -= 1;
        break;
    }
}

void DebugPlayer::key_up(KeyCode key)
{
    Player::key_up(key);
    
    switch (key)
    {
    case KeyCode::W:
        move_forward_ -= 1;
        break;
    case KeyCode::S:
        move_forward_ += 1;
        break;
    case KeyCode::D:
        move_right_ -= 1;
        break;
    case KeyCode::A:
        move_right_ += 1;
        break;
    case KeyCode::Space:
        move_up_ -= 1;
        break;
    case KeyCode::LeftShift:
        move_up_ += 1;
        break;
    }
}

void DebugPlayer::mouse_button_down(int button)
{
    Player::mouse_button_down(button);
    
    if (button == 1)
    {
        if (auto world = get_world())
        {
            /*const auto arrow = MakeShared<Arrow>();
            world->spawn_entity(arrow, get_location() + get_rotation().forward(), get_rotation());*/
        }
    }
}

void DebugPlayer::on_possess()
{
    Player::on_possess();
    
    Game::set_mouse_grab(true);
}

void DebugPlayer::on_tick(float delta_time)
{
    yaw_ += Game::get_mouse_delta().x / 10.0f;
    pitch_ += Game::get_mouse_delta().y / 10.0f;
    pitch_ = Math::clamp(pitch_, -89.f, 89.f);
    set_rotation(Quaternion(Vector3(0, pitch_, yaw_)));
    
    static float speed = 500;
    auto pos = get_location();
    pos += get_rotation().forward() * delta_time * move_forward_ * speed;
    pos += get_rotation().right() * delta_time * move_right_ * speed;
    pos += Vector3::up() * delta_time * move_up_ * speed;
    set_location(pos);

    const ChunkIndex current_chunk = ChunkIndex::from_vector(get_location());

    if (current_chunk != old_chunk_)
    {
        /*if (observer_)
        {
            observer_->move(current_chunk, load_distance_);
            debug_text_->set_text(String::format("Chunk: %i, %i", current_chunk.x, current_chunk.y));
        }*/
        old_chunk_ = current_chunk;
    }

    if (auto world = get_world())
    {
        if (auto hit = world->raycast(get_location(), get_location() + get_rotation().forward() * 10000))
        {
            arrows_->set_location(hit->location);
            arrows_->set_rotation(Quaternion::look_at(hit->normal));
        }
        else
        {
            arrows_->set_location(get_location() + get_rotation().forward() * 200);
            arrows_->set_rotation(Quaternion());
        }
    }
}
