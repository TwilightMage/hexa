#include "DebugPlayer.h"

#include "Arrow.h"
#include "Engine/Game.h"
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
        /*const auto arrows_mesh = StaticMesh::load_file_obj(RESOURCES_ENGINE_MESHES + "axis_arrows.obj");
        auto arrows_vertices = arrows_mesh->get_vertices();
        auto arrows_indices = arrows_mesh->get_indices();
        
        arrows_ = MakeShared<MeshEntity>(MakeShared<StaticMesh>("Axis Arrows", arrows_vertices, arrows_indices));
        arrows_->get_material_instance()->set_param_value("texture", Texture::load_png(RESOURCES_ENGINE_TEXTURES + "axis_arrows.png"));
        arrows_->set_scale(Vector3(0.1f));
        world->spawn_entity(arrows_);*/

        if (auto hexa_world = cast<HexaWorld>(world))
        {
            const auto current_chunk = ChunkIndex::from_vector(get_location());
            
            observer_ = hexa_world->register_chunk_observer(current_chunk, load_distance_);
            observer_->set_render_chunks(true);
        }
    }
}

void DebugPlayer::key_down(int key)
{
    Player::key_down(key);
    
    if (key == int('w')) move_forward_ += 1;
    else if (key == int('s')) move_forward_ -= 1;
    if (key == int('d')) move_right_ += 1;
    else if (key == int('a')) move_right_ -= 1;
    if (key == OgreBites::SDLK_SPACE) move_up_ += 1;
    else if (key == OgreBites::SDLK_LSHIFT) move_up_ -= 1;
}

void DebugPlayer::key_up(int key)
{
    Player::key_up(key);
    
    if (key == int('w')) move_forward_ -= 1;
    else if (key == int('s')) move_forward_ += 1;
    if (key == int('d')) move_right_ -= 1;
    else if (key == int('a')) move_right_ += 1;
    if (key == OgreBites::SDLK_SPACE) move_up_ -= 1;
    else if (key == OgreBites::SDLK_LSHIFT) move_up_ += 1;
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
    
    Game::lock_mouse();
    Game::hide_mouse();
}

void DebugPlayer::on_tick(float delta_time)
{
    static float speed = 300;
    auto pos = get_location();
    pos += get_rotation().forward() * delta_time * move_forward_ * speed;
    pos += get_rotation().right() * delta_time * move_right_ * speed;
    pos.y += delta_time * move_up_ * speed;
    set_location(pos);

    auto rot = get_rotation();
    rot = rot.rotate_around_up(-Game::get_mouse_delta().x / 10.0f).normalized();
    rot = rot.rotate_around(rot.right(), -Game::get_mouse_delta().y / 10.0f).normalized();
    set_rotation(rot);

    const ChunkIndex current_chunk = ChunkIndex::from_vector(get_location());

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
        if (auto hit = world->raycast(get_location(), get_location() + get_rotation().forward() * 10))
        {
            //arrows_->set_location(hit->location);
        }
        else
        {
            //arrows_->set_location(get_location() + get_rotation().forward());
        }
    }
}
