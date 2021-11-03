#include "DebugPlayer.h"

#include "Engine/Game.h"
#include "Engine/Material.h"
#include "Engine/MeshComponent.h"
#include "Engine/World.h"
#include "Engine/Physics/RaycastResult.h"
#include "Engine/ui/TextBlock.h"
#include "HexaGame/Paths.h"
#include "HexaGame/WorldChunkObserver.h"
#include "HexaGame/Worlds/HexaWorld.h"

void DebugPlayer::on_start()
{
    Player::on_start();
    
    if (auto world = get_world())
    {
        auto arrows_mat = Game::get_instance()->get_material("Hexa/Axis_Arrows");
        if (!arrows_mat)
        {
            arrows_mat = Game::get_instance()->get_material("Hexa/Basic")->clone("Hexa/Axis_Arrows");
            arrows_mat->set_texture("axis_arrows.png", 0);
        }
        
        const auto arrows_mesh = StaticMesh::load_file_obj(RESOURCES_MESHES + "axis_arrows.obj");
        
        arrows_ = world->spawn_entity<MeshEntity>(Transform(Vector3(), Quaternion(), Vector3(0.1f)), arrows_mesh);
        arrows_->mesh()->set_material(arrows_mat, 0);

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
    static float speed = 500;
    auto pos = get_location();
    pos += get_rotation().forward() * delta_time * move_forward_ * speed;
    pos += get_rotation().right() * delta_time * move_right_ * speed;
    pos += Vector3::up() * delta_time * move_up_ * speed;
    set_location(pos);

    auto rot = get_rotation();
    rot = rot.rotate_around_up(Game::get_mouse_delta().x / 10.0f).normalized();
    rot = rot.rotate_around(rot.right(), Game::get_mouse_delta().y / 10.0f).normalized();
    set_rotation(rot);

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
            arrows_->set_rotation(Quaternion::look_at(Vector3::zero(), hit->normal));
        }
        else
        {
            arrows_->set_location(get_location() + get_rotation().forward() * 200);
            arrows_->set_rotation(Quaternion());
        }
    }
}
