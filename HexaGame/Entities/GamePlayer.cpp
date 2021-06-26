#include "GamePlayer.h"

#include <GLFW/glfw3.h>

#include "ItemDrop.h"
#include "MeshEntity.h"
#include "Characters/Slime.h"
#include "Engine/Animation.h"
#include "Engine/AnimatorComponent.h"
#include "Engine/Camera.h"
#include "Engine/GeometryEditor.h"
#include "Engine/SystemIO.h"
#include "Engine/Physics/RaycastResult.h"
#include "HexaGame/ChunkMeshEntity.h"
#include "HexaGame/HexaCollisionMaskBits.h"
#include "HexaGame/HexaMath.h"
#include "HexaGame/TileIndex.h"
#include "HexaGame/Tiles.h"
#include "HexaGame/WorldChunk.h"
#include "HexaGame/WorldChunkObserver.h"
#include "HexaGame/WorldPath.h"
#include "HexaGame/Entities/Character.h"
#include "HexaGame/Worlds/HexaWorld.h"

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

float weight_calc(const TileIndex& from, const TileIndex& to)
{
    return Vector3::distance(from.to_vector(), to.to_vector()) * 2;
}

List<Shared<Entity>> markers;
void GamePlayer::mouse_button_down(int button)
{
    if (button == 0)
    {
        if (auto world = cast<HexaWorld>(get_world()))
        {
            for (auto& marker : markers)
            {
                marker->destroy();
            }
            markers.Clear();
            
            if (auto hit = world->raycast(get_position(), get_position() + Game::get_un_projected_mouse() * 10, HexaCollisionMaskBits::GROUND | HexaCollisionMaskBits::ITEM))
            {
                if (cast<ChunkMeshEntity>(hit->entity))
                {
                    const TileIndex tile_index = TileIndex::from_vector(hit->location + hit->normal * 0.1f);
                    if (tile_index != get_character()->get_tile_position())
                    {
                        if (cast<ChunkMeshEntity>(hit->entity))
                        {
                            get_character()->go_to(tile_index);
                            if (auto path = world->FindPath(get_character()->get_path_config(tile_index)))
                            {
                                auto marker_mesh = GeometryEditor::get_unit_cube();
                                auto marker = MakeShared<MeshEntity>(marker_mesh);
                                marker->set_scale(Vector3(0.1f));
                                world->spawn_entity(marker, path->segments.first().from.to_vector() + Vector3(0, 0, HexaMath::tile_height / 2));
                                markers.Add(marker);
                                for (const auto& segment : path->segments)
                                {
                                    marker = MakeShared<MeshEntity>(marker_mesh);
                                    marker->set_scale(Vector3(0.1f));
                                    world->spawn_entity(marker, segment.to.to_vector() + Vector3(0, 0, HexaMath::tile_height / 2));
                                    markers.Add(marker);
                                }
                            }
                        }
                    }
                }
                else if (auto item_drop = cast<ItemDrop>(hit->entity))
                {
                    auto item = item_drop->get_item();
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

    if (auto world = cast<HexaWorld>(get_world()))
    {
        if (auto hit = world->raycast(get_position(), get_position() + Game::get_un_projected_mouse() * 10, HexaCollisionMaskBits::GROUND))
        {
            const TileIndex tile_index = TileIndex::from_vector(hit->location - hit->normal * 0.01f);
            if (auto character = get_character())
            {
                if (!(character->get_tile_position().x == tile_index.x && character->get_tile_position().y == tile_index.y))
                {
                    character->rotate_to_tile(tile_index);
                }
            }
        }
    }
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
                const auto character = MakeShared<Slime>();
                if (world->spawn_character(character, TileIndex(0, 0, WorldChunk::chunk_height - i)))
                {
                    posses_character(character);
                    set_position(character->get_tile_position().to_vector() + Vector3(-4, 0, 4));
                }
                break;
            }
        }
    }

    Game::possess(cast<GamePlayer>(shared_from_this()));
}
