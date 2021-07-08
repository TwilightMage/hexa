#include "GamePlayer.h"

#include <GLFW/glfw3.h>

#include "AxisArrows.h"
#include "ComplexTile.h"
#include "ItemDrop.h"
#include "MeshEntity.h"
#include "Characters/Slime.h"
#include "Engine/Animation.h"
#include "Engine/AnimatorComponent.h"
#include "Engine/Camera.h"
#include "Engine/GeometryEditor.h"
#include "Engine/Material3D.h"
#include "Engine/SystemIO.h"
#include "Engine/Physics/RaycastResult.h"
#include "HexaGame/CharacterInventory.h"
#include "HexaGame/ChunkMeshEntity.h"
#include "HexaGame/HexaCollisionMaskBits.h"
#include "HexaGame/HexaGame.h"
#include "HexaGame/HexaMath.h"
#include "HexaGame/Items.h"
#include "HexaGame/TileIndex.h"
#include "HexaGame/Tiles.h"
#include "HexaGame/WorldChunk.h"
#include "HexaGame/WorldChunkObserver.h"
#include "HexaGame/WorldPath.h"
#include "HexaGame/Entities/Character.h"
#include "HexaGame/ui/Toolbar.h"
#include "HexaGame/Worlds/HexaWorld.h"

void GamePlayer::on_start()
{
    Player::on_start();
    
    set_rotation(Quaternion(Vector3(0, 30, 0)));
    
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

Map<int, uint> hotbar = {
    {GLFW_KEY_1, 0},
    {GLFW_KEY_2, 1},
    {GLFW_KEY_3, 2},
    {GLFW_KEY_4, 3},
    {GLFW_KEY_5, 4},
    {GLFW_KEY_6, 5},
    {GLFW_KEY_7, 6},
    {GLFW_KEY_8, 7},
    {GLFW_KEY_9, 8},
    {GLFW_KEY_0, 9}
};

void GamePlayer::key_down(int key)
{
    Player::key_down(key);
    
    if (key == GLFW_KEY_W) move_.x += 1;
    else if (key == GLFW_KEY_S) move_.x -= 1;
    else if (key == GLFW_KEY_D) move_.y += 1;
    else if (key == GLFW_KEY_A) move_.y -= 1;
    else if (key == GLFW_KEY_LEFT_SHIFT) use_item_mode_ = true;
    else if (hotbar.contains(key))
    {
        if (const auto& character = get_character())
        {
            character->get_inventory()->set_selected_hotbar(hotbar[key]);
        }
    }
}

void GamePlayer::key_up(int key)
{
    Player::key_up(key);
    
    if (key == GLFW_KEY_W) move_.x -= 1;
    else if (key == GLFW_KEY_S) move_.x += 1;
    else if (key == GLFW_KEY_D) move_.y -= 1;
    else if (key == GLFW_KEY_A) move_.y += 1;
    else if (key == GLFW_KEY_LEFT_SHIFT) use_item_mode_ = false;
}

List<Shared<Entity>> markers;
void GamePlayer::mouse_button_down(int button)
{
    Player::mouse_button_down(button);
    
    if (button == 0)
    {
        if (auto world = cast<HexaWorld>(get_world()))
        {
            for (auto& marker : markers)
            {
                marker->destroy();
            }
            markers.clear();

            if (get_character())
            {
                auto raycast_mask = HexaCollisionMaskBits::GROUND | HexaCollisionMaskBits::ITEM;
                if (use_item_mode_)
                {
                    raycast_mask |= HexaCollisionMaskBits::COMPLEX_NOBLOCK | HexaCollisionMaskBits::COMPLEX_BLOCK;
                }
                auto hits = world->raycast_all(camera_position_, camera_position_ + Game::get_un_projected_mouse() * 10, raycast_mask, true);
                for (const auto& hit : hits)
                {
                    if (hit.location.z > world->get_cap_chunk_z() * HexaMath::tile_height + KINDA_SMALL_NUMBER) continue;

                    const auto& selected_item = get_character()->get_inventory()->get_selected_item();
                    
                    if (cast<ChunkMeshEntity>(hit.entity))
                    {
                        const ItemTileTarget tile_target = use_item_mode_ && selected_item.item ? selected_item.item->tile_target : TARGET_OUTSIDE;
                        const TileIndex tile_index = TileIndex::from_vector(hit.location + hit.normal * KINDA_SMALL_NUMBER * (tile_target == TARGET_OUTSIDE ? 1.0f : -1.0f));
                        if (tile_index != get_character()->get_tile_position())
                        {
                            if (use_item_mode_)
                            {
                                if (selected_item.item)
                                {
                                    auto item_copy = selected_item;
                                    selected_item.item->apply_to_tile(item_copy, get_character(), tile_index, cast<HexaWorld>(get_world()));
                                    if (item_copy != selected_item)
                                    {
                                        get_character()->get_inventory()->set_item(get_character()->get_inventory()->get_selected_hotbar(), item_copy);
                                    }
                                }
                            }
                            else
                            {
                                get_character()->go_to(tile_index);
                                if (auto path = world->FindPath(get_character()->get_path_config(tile_index)))
                                {
                                    auto marker_mesh = GeometryEditor::get_unit_cube();
                                    auto marker = MakeShared<MeshEntity>(marker_mesh);
                                    marker->set_scale(Vector3(0.1f));
                                    world->spawn_entity(marker, path->segments.first().from.to_vector() + Vector3(0, 0, HexaMath::tile_height / 2));
                                    markers.add(marker);
                                    for (const auto& segment : path->segments)
                                    {
                                        marker = MakeShared<MeshEntity>(marker_mesh);
                                        marker->set_scale(Vector3(0.1f));
                                        world->spawn_entity(marker, segment.to.to_vector() + Vector3(0, 0, HexaMath::tile_height / 2));
                                        markers.add(marker);
                                    }
                                }
                            }
                        }
                    }
                    else if (cast<ComplexTile>(hit.entity))
                    {
                        if (use_item_mode_ && selected_item.item)
                        {
                            const TileIndex tile_index = TileIndex::from_vector(hit.location + hit.normal * -KINDA_SMALL_NUMBER);
                            if (tile_index != get_character()->get_tile_position())
                            {
                                auto item_copy = selected_item;
                                selected_item.item->apply_to_tile(item_copy, get_character(), tile_index, cast<HexaWorld>(get_world()));
                                if (item_copy != selected_item)
                                {
                                    get_character()->get_inventory()->set_item(get_character()->get_inventory()->get_selected_hotbar(), item_copy);
                                }
                            }
                        }
                    }
                    else if (auto item_drop = cast<ItemDrop>(hit.entity))
                    {
                        const auto item_tile = TileIndex::from_vector(item_drop->get_position());
                        const auto character = get_character();
                        if (TileIndex::distance_xy(item_tile, character->get_tile_position()) <= 1 && Math::abs(item_tile.z - character->get_tile_position().z) <= 1)
                        {
                            if (character->get_inventory()->add_item(item_drop->get_item()))
                            {
                                item_drop->destroy();
                            }
                        }
                    }

                    break;
                }
            }
        }
    }
    else if (button == 2)
    {
        rotate_camera_ = true;
    }
}

void GamePlayer::mouse_button_up(int button)
{
    if (button == 2)
    {
        rotate_camera_ = false;
    }
}

void GamePlayer::scroll(const Vector2& delta)
{
    desired_camera_distance_ = Math::clamp(desired_camera_distance_ - delta.y, 1.0f, 8.0f);
}

void GamePlayer::tick(float delta_time)
{    
    if (auto world = cast<HexaWorld>(get_world()))
    {
        auto pos = get_position();
        if (auto character = get_character())
        {
            static float speed = 3;
            const auto cam_rot_xy = Quaternion(Vector3(0, 0, camera_rotation_.yaw()));
            pos += cam_rot_xy.right().cross_product(Vector3::up()) * delta_time * move_.x * speed;
            pos += cam_rot_xy.right() * delta_time * move_.y * speed;
    
            Vector3 tile_check_from = pos;
            tile_check_from.z = WorldChunk::chunk_height * HexaMath::tile_height;
            Vector3 tile_check_to = pos;
            tile_check_to.z = character->get_tile_position().z * HexaMath::tile_height - KINDA_SMALL_NUMBER;
            auto hits = world->raycast_all(tile_check_from, tile_check_to, HexaCollisionMaskBits::GROUND, true);
            bool found_hit = false;
            if (hits.length() > 0)
            {
                for (const auto& hit : hits)
                {
                    if (hit.location.z > world->get_cap_chunk_z() * HexaMath::tile_height + KINDA_SMALL_NUMBER) continue;

                    pos.z = hit.location.z;
                    found_hit = true;
                }
            }
            
            if (!found_hit)
            {
                pos.z = tile_check_to.z;
            }
            
            set_position(pos);
            desired_camera_pivot_z = pos.z;
        }
        
        if (rotate_camera_)
        {
            Game::get_mouse_delta();

            camera_rotation_ = Quaternion(Vector3(0, Math::clamp(camera_rotation_.pitch() + Game::get_mouse_delta().y * 0.25f, 10.0f, 89.0f), camera_rotation_.yaw() + Game::get_mouse_delta().x * 0.25f));
        }
        else
        {
            auto hits = world->raycast_all(camera_position_, camera_position_ + Game::get_un_projected_mouse() * 10, HexaCollisionMaskBits::GROUND, true);
            for (const auto& hit : hits)
            {
                if (hit.location.z > world->get_cap_chunk_z() * HexaMath::tile_height + KINDA_SMALL_NUMBER) continue;

                const ItemTileTarget tile_target = use_item_mode_ && get_character() && get_character()->get_inventory()->get_selected_item().item ? get_character()->get_inventory()->get_selected_item().item->tile_target : TARGET_OUTSIDE;
                const TileIndex tile_index = TileIndex::from_vector(hit.location + hit.normal * 0.1f * (tile_target == TARGET_OUTSIDE ? 1.0f : -1.0f));
                if (auto character = get_character())
                {
                    if (!(character->get_tile_position().x == tile_index.x && character->get_tile_position().y == tile_index.y))
                    {
                        character->rotate_to_tile(tile_index);
                    }
                }
                
                break;
            }
        }

        camera_pivot_z_ = Math::lerp(camera_pivot_z_, desired_camera_pivot_z, delta_time * 10);
        camera_distance_ = Math::lerp(camera_distance_, desired_camera_distance_, delta_time * 10);
        Vector3 camera_pivot = Vector3(pos.x, pos.y, camera_pivot_z_);
        camera_position_ = camera_pivot - camera_rotation_.forward() * camera_distance_;
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

CameraInfo GamePlayer::get_camera_info() const
{
    return {
        camera_position_,
        camera_rotation_
    };
}

void GamePlayer::spawn_chunk_loaded(const Shared<WorldChunk>& sender)
{
    sender->on_loaded.unbind(this, &GamePlayer::spawn_chunk_loaded);

    if (auto world = cast<HexaWorld>(get_world()))
    {
        for (uint i = 0; i < WorldChunk::chunk_height; i++)
        {
            if (!(sender->get_tile(TileIndex(0, 0, WorldChunk::chunk_height - 1 - i))->type & (TileType::Air | TileType::Complex)))
            {
                const auto character = MakeShared<Slime>();
                
                auto toolbar_ui = MakeShared<Toolbar>();
                toolbar_ui->bind(character->get_inventory());
                Game::add_ui(toolbar_ui);
                
                if (world->spawn_character(character, TileIndex(0, 0, WorldChunk::chunk_height - i)))
                {
                    character->on_tile_position_changed.bind(this, &GamePlayer::character_position_changed);

                    world->get_chunk(character->get_tile_position().get_chunk())->on_tile_change.bind(this, &GamePlayer::current_chunk_tile_changed);
                    
                    posses_character(character);
                    set_position(character->get_tile_position().to_vector());
                    camera_pivot_z_ = desired_camera_pivot_z = character->get_tile_position().z * HexaMath::tile_height;

                    world->spawn_drop(TileIndex(0, 1, WorldChunk::chunk_height - i + 1), ItemContainer(Items::iron_shovel));
                    world->spawn_drop(TileIndex(1, 1, WorldChunk::chunk_height - i + 1), ItemContainer(Items::stone_bricks));
                    world->spawn_drop(TileIndex(1, 1, WorldChunk::chunk_height - i + 1), ItemContainer(Items::stone_bricks));
                    world->spawn_drop(TileIndex(1, 1, WorldChunk::chunk_height - i + 1), ItemContainer(Items::stone_bricks));
                }
                break;
            }
        }
    }

    Game::possess(cast<GamePlayer>(shared_from_this()));
}

void GamePlayer::character_position_changed(const Shared<Character>& sender)
{
    const auto new_chunk = sender->get_tile_position().get_chunk();
    if (new_chunk != old_chunk_)
    {
        if (auto world = cast<HexaWorld>(get_world()))
        {
            world->get_chunk(old_chunk_)->on_tile_change.unbind(this, &GamePlayer::current_chunk_tile_changed);
            world->get_chunk(new_chunk)->on_tile_change.bind(this, &GamePlayer::current_chunk_tile_changed);
        }
        
        observer_->move(new_chunk, load_distance_);
        old_chunk_ = new_chunk;
    }

    check_cap();
}

void GamePlayer::current_chunk_tile_changed(const ChunkIndex& chunk, const TileIndex& tile)
{
    check_cap();
}

void GamePlayer::check_cap()
{
    if (auto character = get_character())
    {
        if (auto world = cast<HexaWorld>(get_world()))
        {
            bool cap = false;
            for (uint i = 0; i < 10; i++)
            {
                if (!!(world->get_tile_id(character->get_tile_position().offset(0, 0, i))->type & ~(TileType::Air | TileType::None | TileType::Complex)))
                {
                    cap = true;
                    break;
                }
            }
        
            if (cap)
            {
                world->cap_chunks(character->get_tile_position().z + character->declared_height);
            }
            else
            {
                world->uncap_chunks();
            }
        }
    }
}
