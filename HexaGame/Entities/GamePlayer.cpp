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
#include "HexaGame/CharacterInventory.h"
#include "HexaGame/ChunkMeshEntity.h"
#include "HexaGame/HexaCollisionMaskBits.h"
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
            markers.Clear();

            if (get_character())
            {
                if (auto hit = world->raycast(get_position(), get_position() + Game::get_un_projected_mouse() * 10, HexaCollisionMaskBits::GROUND | HexaCollisionMaskBits::ITEM))
                {
                    if (cast<ChunkMeshEntity>(hit->entity))
                    {
                        const auto& selected_item = get_character()->get_inventory()->get_selected_item();
                        const ItemTileTarget tile_target = use_item_mode_ && selected_item.item ? selected_item.item->tile_target : TARGET_OUTSIDE;
                        const TileIndex tile_index = TileIndex::from_vector(hit->location + hit->normal * 0.1f * (tile_target == TARGET_OUTSIDE ? 1.0f : -1.0f));
                        if (tile_index != get_character()->get_tile_position())
                        {
                            if (cast<ChunkMeshEntity>(hit->entity))
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
                    }
                    else if (auto item_drop = cast<ItemDrop>(hit->entity))
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
                }
            }
        }
    }
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
            const ItemTileTarget tile_target = use_item_mode_ && get_character() && get_character()->get_inventory()->get_selected_item().item ? get_character()->get_inventory()->get_selected_item().item->tile_target : TARGET_OUTSIDE;
            const TileIndex tile_index = TileIndex::from_vector(hit->location + hit->normal * 0.1f * (tile_target == TARGET_OUTSIDE ? 1.0f : -1.0f));
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

                auto toolbar_ui = MakeShared<Toolbar>();
                toolbar_ui->bind(character->get_inventory());
                Game::add_ui(toolbar_ui);
                
                if (world->spawn_character(character, TileIndex(0, 0, WorldChunk::chunk_height - i)))
                {
                    old_chunk_ = character->get_tile_position().get_chunk();
                    character->on_tile_position_changed.bind(this, &GamePlayer::character_position_changed);
                    
                    posses_character(character);
                    set_position(character->get_tile_position().to_vector() + Vector3(-4, 0, 4));

                    world->spawn_drop(TileIndex(0, 1, WorldChunk::chunk_height - i), ItemContainer(Items::iron_shovel));
                    world->spawn_drop(TileIndex(-1, 1, WorldChunk::chunk_height - i), ItemContainer(Items::stone_bricks));
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
        observer_->move(new_chunk, load_distance_);
        old_chunk_ = new_chunk;
    }
}
