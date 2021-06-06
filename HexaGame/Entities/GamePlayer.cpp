#include "GamePlayer.h"

#include <GLFW/glfw3.h>

#include "Engine/Camera.h"
#include "Engine/Physics/RaycastResult.h"
#include "HexaGame/Character.h"
#include "HexaGame/ChunkMeshEntity.h"
#include "HexaGame/HexaMath.h"
#include "HexaGame/HexaWorld.h"
#include "HexaGame/TileIndex.h"
#include "HexaGame/Tiles.h"
#include "HexaGame/WorldChunk.h"
#include "HexaGame/WorldChunkObserver.h"

void GamePlayer::on_start()
{
    set_rotation(Quaternion(Vector3(0, 30, 0)));
    
    camera_ = MakeShared<Camera>();
    camera_->owner = this;
    camera_->fov = 45.0f;
    
    if (auto world = cast<HexaWorld>(get_world()))
    {
        const auto current_chunk = ChunkIndex::from_vector(get_position());
            
        observer_ = world->register_chunk_observer(current_chunk, load_distance_);
        observer_->set_render_chunks(true);

        const auto spawn_chunk = world->get_chunk(ChunkIndex::from_vector(get_position()));
        if (spawn_chunk->get_state() == WorldChunkDataState::Loaded)
        {
            spawn_chunk_loaded(spawn_chunk);
        }
        else
        {
            spawn_chunk->on_loaded.bind(this, &GamePlayer::spawn_chunk_loaded);
        }
    }
}

void GamePlayer::key_down(int key)
{
    if (key == GLFW_KEY_W) move_.x += 1;
    else if (key == GLFW_KEY_S) move_.x -= 1;
    if (key == GLFW_KEY_D) move_.y += 1;
    else if (key == GLFW_KEY_A) move_.y -= 1;
}

void GamePlayer::key_up(int key)
{
    if (key == GLFW_KEY_W) move_.x -= 1;
    else if (key == GLFW_KEY_S) move_.x += 1;
    if (key == GLFW_KEY_D) move_.y -= 1;
    else if (key == GLFW_KEY_A) move_.y += 1;
}

void GamePlayer::mouse_button_down(int button)
{
    if (button == 0)
    {
        if (auto world = cast<HexaWorld>(get_world()))
        {
            if (auto hit = world->raycast(get_position(), get_position() + Game::get_un_projected_mouse() * 10))
            {
                if (cast<ChunkMeshEntity>(hit->entity))
                {
                    world->set_tile(TileIndex::from_vector(hit->location - hit->normal * 0.1f), Tiles::air);
                }
            }
        }
    }
}

void GamePlayer::on_possess()
{
    Game::use_camera(camera_);
}

void GamePlayer::tick(float delta_time)
{
    static float speed = 3;
    auto pos = get_position();
    pos += get_rotation().right().cross_product(Vector3::up()) * delta_time * move_.x * speed;
    pos += get_rotation().right() * delta_time * move_.y * speed;
    set_position(pos);
}

Shared<const CharacterController> GamePlayer::get_as_character_controller() const
{
    return cast<const CharacterController>(shared_from_this());
}

Shared<CharacterController> GamePlayer::get_as_character_controller()
{
    return cast<CharacterController>(shared_from_this());
}

void GamePlayer::spawn_chunk_loaded(const Shared<WorldChunk>& sender)
{
    sender->on_loaded.unbind(this, &GamePlayer::spawn_chunk_loaded);

    if (auto world = cast<HexaWorld>(get_world()))
    {
        for (uint i = 0; i < WorldChunk::chunk_height; i++)
        {
            if (sender->get_tile(TileIndex(0, 0, WorldChunk::chunk_height - 1 - i)) != Tiles::air)
            {
                const auto character = MakeShared<Character>();
                if (world->spawn_character(character, TileIndex(0, 0, WorldChunk::chunk_height - i)))
                {
                    posses_character(character);
                    set_position(TileIndex(0, 0, WorldChunk::chunk_height - 1 - i + 1 + 10).to_vector() + Vector3(0, 0, HexaMath::tile_height / 2));
                }
            
                Game::possess(cast<GamePlayer>(shared_from_this()));
                break;
            }
        }
    }
}
