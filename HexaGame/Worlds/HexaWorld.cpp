#include "HexaWorld.h"

#include "Engine/Game.h"
#include "Engine/Logger.h"
#include "Engine/Math.h"
#include "Engine/Random.h"
#include "Engine/Rect.h"
#include "HexaGame/HexaSaveGame.h"
#include "HexaGame/WorldChunk.h"
#include "HexaGame/WorldChunkObserver.h"
#include "HexaGame/WorldGenerator.h"
#include "HexaGame/WorldPath.h"
#include "HexaGame/Database/items/ItemInfo.h"
#include "HexaGame/Entities/Character.h"
#include "HexaGame/Entities/ItemDrop.h"

HexaWorld::HexaWorld(const Shared<WorldGenerator>& generator)
    : World()
    , generator_(generator)
    , cap_chunk_z(WorldChunk::chunk_height)
{

}

const Shared<WorldGenerator>& HexaWorld::get_generator() const
{
    return generator_;
}

Shared<WorldChunkObserver> HexaWorld::register_chunk_observer(const Rect& rect)
{
    auto observer = MakeSharedInternal(WorldChunkObserver, rect, cast<HexaWorld>(weak_from_this()));

    collect_observer_array(observer->chunks_, rect.x, rect.y);
    fill_observer_array(observer->chunks_, rect.x, rect.y);

    for (uint x = 0; x < observer->chunks_.get_size_x(); x++)
    {
        for (uint y = 0; y < observer->chunks_.get_size_y(); y++)
        {
            observer->chunks_.at(x, y)->inc_observe();
        }
    }
    
    chunk_observers_.add(observer);

    return observer;
}

Shared<WorldChunkObserver> HexaWorld::register_chunk_observer(const ChunkIndex& chunk_index, uint half_size)
{
    return register_chunk_observer(Rect(chunk_index.x - half_size, chunk_index.y - half_size, half_size * 2 + 1, half_size * 2 + 1));
}

Shared<WorldChunk> HexaWorld::get_chunk(const ChunkIndex& chunk_index) const
{
    return get_chunk_internal(chunk_index);
}

// pathfinding

struct PathMove
{
    TileIndex from;
    TileIndex to;
    float weight;
};

struct PathFindingData
{
    List<PathMove> moves;
    Map<TileIndex, bool> visited;
    PathConfig config;
    Vector3 to_vector;
    HexaWorld* world;
};

bool build_path_recursive(PathFindingData& data, const PathMove& current_move, bool is_first);

float weight_moves(const List<PathMove>& moves)
{
    float result = 0;
    for (const auto& move : moves)
    {
        result += move.weight;
    }

    return result;
}

bool attempt_move(PathFindingData& data, const TileIndex& current_tile, const TileIndex& next_tile, bool vertical_priority)
{
    if (build_path_recursive(data, PathMove(current_tile, next_tile, 1), false)) return true;

    // try to jump down/climb
    for (byte v = 0; v < 2; v++)
    {
        // try to jump down
        if (v == (byte)vertical_priority)
        {
            bool enough_space_to_jump_down = true;
            for (uint j = 0; j < data.config.agent_height; j++)
            {
                if (!(data.world->get_tile_id(next_tile.offset(0, 0, data.config.agent_height - 1 - j))->type & (TileType::Air | TileType::Complex)))
                {
                    enough_space_to_jump_down = false;
                }
            }

            if (enough_space_to_jump_down)
            {
                TileIndex jump_to;
                bool has_land_to_jump_down = false;
                
                for (int j = 1; j < (int)data.config.allowed_fall + 1; j++)
                {
                    jump_to = next_tile.offset(0, 0, -j - 1);
                    if (data.world->get_tile_id(jump_to)->type == TileType::Solid)
                    {
                        has_land_to_jump_down = true;
                        jump_to = jump_to.offset(0, 0, 1);
                        break;
                    }
                }

                if (has_land_to_jump_down)
                {
                    if (build_path_recursive(data, PathMove(current_tile, jump_to, (current_tile.z - jump_to.z) * 1.2f), false)) return true;
                }
            }
        }

        // try to climb
        if (v != (byte)vertical_priority)
        {
            TileIndex climb_on;
            bool has_land_to_climb_on = false;
            uint space = 0;
            for (uint i = 1; i < data.config.allowed_climb + data.config.agent_height; i++)
            {
                if (!!(data.world->get_tile_id(next_tile.offset(0, 0, i))->type & (TileType::Air | TileType::Complex)))
                {
                    if (++space == data.config.agent_height)
                    {
                        climb_on = next_tile.offset(0, 0, i - data.config.agent_height + 1);
                        has_land_to_climb_on = true;
                        break;
                    }
                }
                else if (space > 0)
                {
                    break;
                }
            }

            if (has_land_to_climb_on)
            {
                bool has_space_to_climb = true;
                for (uint i = 1; i < (climb_on.z - current_tile.z) + data.config.agent_height; i++)
                {
                    if (!(data.world->get_tile_id(current_tile.offset(0, 0, i))->type & (TileType::Air | TileType::Complex)))
                    {
                        has_space_to_climb = false;
                        break;
                    }
                }

                if (has_space_to_climb)
                {
                    if (build_path_recursive(data, PathMove(current_tile, climb_on, (climb_on.z - current_tile.z) * 1.2f), false)) return true;
                }
            }
        }
    }

    return false;
}

FORCEINLINE bool can_step_at(const TileIndex& at, uint agent_height, HexaWorld* world)
{
    for (uint i = 0; i < agent_height; i++)
    {
        if (!(world->get_tile_id(at.offset(0, 0, i))->type & (TileType::Air | TileType::Complex))) return false;
    }
    if (world->get_tile_id(at.offset(0, 0, -1))->type != TileType::Solid) return false;
    return true;
}

bool build_path_recursive(PathFindingData& data, const PathMove& current_move, bool is_first)
{
    if (!can_step_at(current_move.to, data.config.agent_height, data.world)) return false;

    if (data.moves.length() > 0 && data.moves.first().from == current_move.to) return false;

    auto& visited = data.visited[current_move.to];
    if (visited) return false;
    visited = true;

    if (!is_first)
    {
        data.moves.add(current_move);
    }

    if (current_move.to == data.config.to)
    {
        return true;
    }

    const float angle_to = current_move.to.x != data.config.to.x || current_move.to.y != data.config.to.y ? -Vector2::angle_global(current_move.to.to_vector(), data.to_vector) : 0.0f;
    const bool vertical_priority = data.config.to.z > current_move.to.z;
    
    const TileSide side_try_f = tile_side_from_angle_xy(angle_to);
    const TileSide side_try_r = tile_side_right(side_try_f);
    const TileSide side_try_l = tile_side_left(side_try_f);
    const TileSide side_try_rr = tile_side_right(side_try_r);
    const TileSide side_try_ll = tile_side_left(side_try_l);
    const TileSide side_try_b = tile_side_right(side_try_rr);
    
    {    
        const TileIndex next_tile_f = current_move.to.offset(side_try_f);

        if (attempt_move(data, current_move.to, next_tile_f, vertical_priority)) return true;
    }
    
    {    
        
        const TileIndex next_tile_r = current_move.to.offset(side_try_r);
        PathFindingData data_r = data;
        const bool move_r = attempt_move(data_r, current_move.to, next_tile_r, vertical_priority);
    
        const TileIndex next_tile_l = current_move.to.offset(side_try_l);
        PathFindingData data_l = data;
        const bool move_l = attempt_move(data_l, current_move.to, next_tile_l, vertical_priority);

        if (move_r && move_l)
        {
            if (weight_moves(data_l.moves) < weight_moves(data_r.moves))
            {
                data = data_l;
            }
            else
            {
                data = data_r;
            }

            return true;
        }
        else if (move_r)
        {
            data = data_r;
            return true;
        }
        else if (move_l)
        {
            data = data_l;
            return true;
        }
    }

    {
        const TileIndex next_tile_rr = current_move.to.offset(side_try_rr);
        PathFindingData data_rr = data;
        const bool move_rr = attempt_move(data_rr, current_move.to, next_tile_rr, vertical_priority);
    
        const TileIndex next_tile_ll = current_move.to.offset(side_try_ll);
        PathFindingData data_ll = data;
        const bool move_ll = attempt_move(data_ll, current_move.to, next_tile_ll, vertical_priority);

        if (move_rr && move_ll)
        {
            if (weight_moves(data_ll.moves) < weight_moves(data_rr.moves))
            {
                data = data_ll;
            }
            else
            {
                data = data_rr;
            }

            return true;
        }
        else if (move_rr)
        {
            data = data_rr;
            return true;
        }
        else if (move_ll)
        {
            data = data_ll;
            return true;
        }
    }
    
    {    
        const TileIndex next_tile_b = current_move.to.offset(side_try_b);

        if (attempt_move(data, current_move.to, next_tile_b, vertical_priority)) return true;
    }

    if (!is_first)
    {
        data.moves.remove_at(data.moves.length() - 1);
    }
    return false;
}

Shared<WorldPath> HexaWorld::FindPath(const PathConfig& config)
{
    if (config.from == config.to) return nullptr;
    
    const ChunkIndex chunk_from = config.from.get_chunk();
    const ChunkIndex chunk_to = config.to.get_chunk();

    if ((uint)Math::abs(chunk_to.x - chunk_from.x) <= config.domain_size || (uint)Math::abs(chunk_to.y - chunk_from.y) <= config.domain_size)
    {
        if (!can_step_at(config.to, config.agent_height, this)) return nullptr;
        
        PathFindingData path_finding_data = { List<PathMove>(), {}, config, config.to.to_vector(), this };
        build_path_recursive(path_finding_data, {config.from, config.from, 0}, true);
        List<WorldPath::Segment> path_segments;
        for (const auto& path_move : path_finding_data.moves)
        {
            path_segments.add({ path_move.from, path_move.to });
        }
        return MakeShared<WorldPath>(path_segments);
    }

    return nullptr;
}

bool HexaWorld::spawn_character(const Shared<Character>& character, const TileIndex& world_index)
{
    if (characters_.contains(character)) return false;
    
    for (auto charact : characters_)
    {
        if (charact->get_tile_position() == world_index) return false;
    }

    character->tile_position_ = world_index;
    
    if (spawn_entity(character, world_index.to_vector()))
    {
        characters_.add(character);

        character->on_destroyed.bind(this, &HexaWorld::character_destroyed_callback);

        on_character_spawned(character);

        return true;
    }

    return false;
}

Shared<Character> HexaWorld::get_character_at(const TileIndex& world_index) const
{
    for (auto& character : characters_)
    {
        if (character->get_tile_position() == world_index)
        {
            return character;
        }
    }

    return nullptr;
}

Shared<Entity> HexaWorld::spawn_drop(const TileIndex& tile, const ItemContainer& item)
{
    if (item.is_empty()) return nullptr;
    
    Shared<Entity> entity = MakeShared<ItemDrop>(item);
    spawn_entity(MakeShared<ItemDrop>(item), tile.to_vector() - Vector3(Random::static_number(-0.2f, 0.2f), Random::static_number(-0.2f, 0.2f), item.item ? (item.item->mesh->get_bounds_center().z - item.item->mesh->get_bounds_half_size().z) : 0.0f), Quaternion(Vector3(0, 0, Random::static_number(360.0f))));
    return entity;
}

void HexaWorld::set_tile(const TileIndex& index, const Shared<const TileInfo>& id) const
{
    const auto chunk_index = index.get_chunk();
    if (auto chunk = get_chunk_internal(chunk_index))
    {
        chunk->set_tile(index.cycle_chunk(), id);
    }
    else
    {
        if (auto save_game = cast<HexaSaveGame>(Game::get_save_game()))
        {
            if (auto modifications = save_game->get_chunk_modifications(chunk_index))
            {
                modifications->insert(index.cycle_chunk(), id);
                save_game->save_chunk_modifications(chunk_index, *modifications);
            }
            else
            {
                Map<TileIndex, Shared<const TileInfo>> new_modifications;
                new_modifications.insert(index.cycle_chunk(), id);
                save_game->save_chunk_modifications(chunk_index, new_modifications);
            }
        }
    }
}

Shared<const TileInfo> HexaWorld::get_tile_id(const TileIndex& index) const
{
    const auto chunk_index = index.get_chunk();
    if (const auto chunk = get_chunk_internal(chunk_index))
    {
        return chunk->get_tile(index.cycle_chunk());
    }

    print_debug("World", "Unable to get tile {$i;$i;$i}, chunk {$i;$i} is not loaded", index.x, index.y, index.z, chunk_index.x, chunk_index.y);
    return nullptr;
}

bool HexaWorld::damage_tile(const TileIndex& index, float damage) const
{
    const auto chunk_index = index.get_chunk();
    if (auto chunk = get_chunk_internal(chunk_index))
    {
        return chunk->damage_tile(index.cycle_chunk(), damage);
    }

    return false;
}

void HexaWorld::cap_chunks(uint z)
{
    if (cap_chunk_z == z) return;
    
    cap_chunk_z = z;

    for (const auto& observer : chunk_observers_)
    {
        for (const auto& chunk : observer->chunks_)
        {
            chunk->cap(z);
        }
    }
}

void HexaWorld::uncap_chunks()
{
    cap_chunks(WorldChunk::chunk_height);
}

void HexaWorld::dump_observable_area()
{
    int min_x = std::numeric_limits<int>::max();
    int min_y = std::numeric_limits<int>::max();
    int max_x = std::numeric_limits<int>::min();
    int max_y = std::numeric_limits<int>::min();
    for (auto observer : chunk_observers_)
    {
        auto& rect = observer->get_rect();
        min_x = Math::min(min_x, rect.x);
        min_y = Math::min(min_y, rect.y);
        max_x = Math::max(max_x, rect.x + rect.w);
        max_y = Math::max(max_y, rect.y + rect.h);
    }

    Array2D<bool> matrix(max_x - min_x, max_y - min_y);
    for (auto observer : chunk_observers_)
    {
        auto& rect = observer->get_rect();
        for (uint i = 0; i < observer->chunks_.get_size_x(); i++)
        {
            for (uint j = 0; j < observer->chunks_.get_size_y(); j++)
            {
                matrix.at(rect.x - min_x + j, rect.y - min_y + i) = observer->chunks_.at(j, i) ? true : false;
            }
        }
    }
    
    List<String> s1;
    for (uint i = 0; i < matrix.get_size_y(); i++)
    {
        List<String> s2;
        for (uint j = 0; j < matrix.get_size_x(); j++)
        {
            if (j + min_x == 0 && i + min_y == 0) s2.add("()");
            else if (j + min_x == 10 && i + min_y == 10) s2.add("XX");
            else s2.add(String::format("%s", matrix.at(j, i) ? "00" : "--"));
        }
        s1.add(String::join(s2, " "));
    }
    String s = String::join(s1, "\n");
    print_debug("Dump loaded chunks", "From %i %i\n" + s, min_x, min_y);
}

void HexaWorld::character_destroyed_callback(const Shared<Entity>& character)
{
    character->on_destroyed.unbind(this, &HexaWorld::character_destroyed_callback);
    
    if (auto charact = cast<Character>(character))
    {
        characters_.remove(charact);
    }
}

Shared<WorldChunk> HexaWorld::get_chunk_internal(const ChunkIndex& chunk_index) const
{
    for (auto observer : chunk_observers_)
    {
        auto& rect = observer->get_rect();
        if (rect.contains(chunk_index.x, chunk_index.y))
        {
            return observer->chunks_.at(chunk_index.x - rect.x, chunk_index.y - rect.y);
        }
    }

    return nullptr;
}

void HexaWorld::unregister_chunk_observer(WorldChunkObserver* observer)
{
    int index = -1;
    for (uint i = 0; i < chunk_observers_.length(); i++)
    {
        if (chunk_observers_[i].get() == observer)
        {
            index = i;
            break;
        }
    }

    if (index == -1) return;

    auto& old_rect = observer->get_rect();
    for (int x = 0; x < old_rect.w; x++)
    {
        for (int y = 0; y < old_rect.h; y++)
        {
            auto& chunk = observer->chunks_.at(x, y);

            chunk->dec_observe();
            if (observer->render_chunks_)
            {
                chunk->dec_visibility();
            }
        }
    }

    chunk_observers_.remove_at(index);
}

void HexaWorld::move_observer(WorldChunkObserver* observer, const Rect& new_rect)
{
    int index = -1;
    for (uint i = 0; i < chunk_observers_.length(); i++)
    {
        if (chunk_observers_[i].get() == observer)
        {
            index = i;
            break;
        }
    }

    if (index == -1) return;
    
    auto& old_chunks = observer->chunks_;
    auto& old_rect = observer->rect_;
    const auto old_rect_render = Rect(old_rect.x + 1, old_rect.y + 1, old_rect.w - 2, old_rect.h - 2);

    Array2D<Shared<WorldChunk>> new_chunks = Array2D<Shared<WorldChunk>>(new_rect.w, new_rect.h);
    const auto new_rect_render = Rect(new_rect.x + 1, new_rect.y + 1, new_rect.w - 2, new_rect.h - 2);

    collect_observer_array(new_chunks, new_rect.x, new_rect.y);
    fill_observer_array(new_chunks, new_rect.x, new_rect.y);

    for (const auto& new_chunk : new_chunks)
    {
        new_chunk->cap(cap_chunk_z);
    }
    
    for (int x = 0; x < old_rect.w; x++)
    {
        for (int y = 0; y < old_rect.h; y++)
        {
            const int g_x = old_rect.x + x;
            const int g_y = old_rect.y + y;
            
            if (!new_rect.contains(g_x, g_y))
            {
                old_chunks.at(x, y)->dec_observe();
            }

            if (observer->render_chunks_)
            {
                if (old_rect_render.contains(g_x, g_y) && !new_rect_render.contains(g_x, g_y))
                {
                    old_chunks.at(x, y)->dec_visibility();
                }
            }
        }
    }
    
    for (int x = 0; x < new_rect.w; x++)
    {
        for (int y = 0; y < new_rect.h; y++)
        {
            const int g_x = new_rect.x + x;
            const int g_y = new_rect.y + y;
            
            if (new_rect.contains(g_x, g_y))
            {
                new_chunks.at(x, y)->inc_observe();
            }

            if (observer->render_chunks_)
            {
                if (new_rect_render.contains(g_x, g_y) && !old_rect_render.contains(g_x, g_y))
                {
                    new_chunks.at(x, y)->inc_visibility();
                }
            }
        }
    }

    old_chunks = new_chunks;
    old_rect = new_rect;
}

void HexaWorld::collect_observer_array(Array2D<Shared<WorldChunk>>& array, int start_x, int start_y)
{
    const Rect rect = Rect(start_x, start_y, array.get_size_x(), array.get_size_y());
    
    for (auto another_observer : chunk_observers_)
        {
            Rect another_rect = another_observer->get_rect();
    
            if (const auto intersection = another_rect.intersection(rect))
            {
                for (int x = 0; x < intersection->w; x++)
                {
                    for (int y = 0; y < intersection->h; y++)
                    {
                       array.at(intersection->x - rect.x + x, intersection->y - rect.y + y) = another_observer->chunks_.at(intersection->x - another_rect.x + x, intersection->y - another_rect.y + y);
                    }
                }
            }
        }
}

void HexaWorld::fill_observer_array(Array2D<Shared<WorldChunk>>& array, int start_x, int start_y)
{
    List<ChunkIndex> added;
    
    for (uint x = 0; x < array.get_size_x(); x++)
    {
        for (uint y = 0; y < array.get_size_y(); y++)
        {
            auto& chunk = array.at(x, y);
            if (chunk == nullptr)
            {
                chunk = MakeShared<WorldChunk>(ChunkIndex(start_x + x, start_y + y), cast<HexaWorld>(shared_from_this()));
                added.add(ChunkIndex(x, y));
            }
        }
    }

    Vector3 mid_chunk = ChunkIndex(array.get_size_x() / 2, array.get_size_y() / 2).to_vector();
    added.sort([&](const ChunkIndex& a, const ChunkIndex& b) -> bool
    {
        return Vector3::distance(a.to_vector(), mid_chunk) > Vector3::distance(b.to_vector(), mid_chunk);
    });

    for (const auto& index : added)
    {
        array.at(index.x, index.y)->load();
    }

    for (auto& index : added)
    {
        const bool _f = index.x < static_cast<int>(array.get_size_x()) - 1; // have local front
        const bool _b = index.x > 0;                                        // have local back
        const bool _r = index.y < static_cast<int>(array.get_size_y()) - 1; // have local right
        const bool _l = index.y > 0;                                        // have local left
        const auto chunk_front = _f
            ? array.at(index.x + 1, index.y)
            : get_chunk(ChunkIndex(start_x + array.get_size_x() + 1, start_y + index.y));
        const auto chunk_back = _b
            ? array.at(index.x - 1, index.y)
            : get_chunk(ChunkIndex(start_x - 1, start_y + index.y));
        const auto chunk_right = _r
            ? array.at(index.x, index.y + 1)
            : get_chunk(ChunkIndex(start_x + index.x, start_y + array.get_size_y() + 1));
        const auto chunk_left = _l
            ? array.at(index.x, index.y - 1)
            : get_chunk(ChunkIndex(start_x + index.x, start_y - 1));
        const auto chunk_front_right = _f
            ? _r
                ? array.at(index.x + 1, index.y + 1)
                : get_chunk(ChunkIndex(start_x + index.x + 1, start_y + array.get_size_y() + 1))
            : _r
                ? get_chunk(ChunkIndex(start_x + array.get_size_x() + 1, start_y + index.y + 1))
                : get_chunk(ChunkIndex(start_x + array.get_size_x() + 1, start_y + array.get_size_y() + 1));
        const auto chunk_back_left = _b
            ? _l
                ? array.at(index.x - 1, index.y - 1)
                : get_chunk(ChunkIndex(start_x + index.x - 1, start_y - 1))
            : _l
                ? get_chunk(ChunkIndex(start_x - 1, start_y + index.y - 1))
                : get_chunk(ChunkIndex(start_x - 1, start_y - 1));
        auto& chunk = array.at(index.x, index.y);
        chunk->link(
            chunk_front ? chunk_front.get() : nullptr,
            chunk_right ? chunk_right.get() : nullptr,
            chunk_front_right ? chunk_front_right.get() : nullptr,
            chunk_back ? chunk_back.get() : nullptr,
            chunk_left ? chunk_left.get() : nullptr,
            chunk_back_left ? chunk_back_left.get() : nullptr
            );
    }
}
