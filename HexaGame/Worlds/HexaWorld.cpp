#include "HexaWorld.h"

#include <queue>

#include "Engine/Game.h"
#include "Engine/Logger.h"
#include "Engine/Math.h"
#include "Engine/Random.h"
#include "Engine/Rect.h"
#include "HexaGame/HexaSaveGame.h"
#include "HexaGame/Nav.h"
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

void HexaWorld::on_start()
{
    set_ambient_light(Color::white(), 0.5f);
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

struct PathFindingData
{
    PathConfig config;
    HexaWorld* world;
    float max_cost;
    List<WorldPath::Segment> moves;
};

FORCEINLINE bool can_step_at(const TileIndex& at, uint agent_height, HexaWorld* world)
{
    const auto chunk = world->get_chunk(at.get_chunk());
    const auto local_at = at.cycle_chunk();
    const auto local_at_down = local_at.offset(0, 0, -1);
    
    if (at.z <= 0 ||
        !(chunk->get_tile(local_at_down)->get_collision_sides(local_at_down, world) & TileSide::Up) &&
        !(chunk->get_tile(local_at)->get_collision_sides(local_at, world) & TileSide::Down)) return false; // have floor
    
    for (uint i = at.z + 1; i < Math::min(at.z + agent_height, WorldChunk::chunk_height - 1); i++)
    {
        const auto index = TileIndex(local_at.x, local_at.y, i);
        const auto index_down = TileIndex(local_at.x, local_at.y, i - 1);
        if (!!(chunk->get_tile(index)->get_collision_sides(index, world) & TileSide::Down) || !!(chunk->get_tile(index_down)->get_collision_sides(index_down, world) & TileSide::Up)) return false;
    }

    return true;
}

#define SORT_BUBBLE_BEGIN(size)\
for (uint i = 0; i < size - 1; i++)\
{\
    for (uint j = i + 1; j < size; j++)\
    {\

#define SORT_BUBBLE_END(list)\
        {\
            std::swap(list[i], list[j]);\
        }\
    }\
}\

struct NodeData
{
    const NavNode* node;
    const NavConnection* from;
    float total_cost;
};

void build_path(PathFindingData& data)
{
    typedef std::pair<float, const NavNode*> pair;
    
    if (const auto start_node = data.world->get_nav_node(data.config.from))
    {
        if (const auto end_node = data.world->get_nav_node(data.config.to))
        {
            std::priority_queue<pair, std::vector<pair>, std::greater<pair>> frontier;
            frontier.push(std::pair{0.0f, start_node});

            Map<const NavNode*, NodeData> node_datas;
            node_datas[start_node] = { start_node, nullptr, 0.0f };

            bool finished = false;
            bool unreachable = false;
            
            while (!frontier.empty() && !finished && !unreachable)
            {
                auto current = frontier.top().second;
                frontier.pop();

                auto current_data = node_datas[current];

                for (auto connection : current->connected_nodes)
                {
                    if (auto next = connection->value->pass(current, data.config))
                    {
                        if (next == start_node) continue;
                        
                        const float next_cost = current_data.total_cost + connection->value->cost(current, data.config);

                        if (can_step_at(next->tile_index, data.config.agent_height, data.world) && next_cost < data.max_cost)
                        {
                            auto& next_data = node_datas[next];
                            next_data.node = next;
                            if (next_data.from == nullptr)
                            {
                                next_data.from = connection->value;
                                next_data.total_cost = next_cost;
                                frontier.emplace(pair{next_data.total_cost, next});

                                if (next == end_node)
                                {
                                    finished = true;
                                }
                            }
                        }
                        else if (next == end_node)
                        {
                            unreachable = true;
                        }
                    }
                }
            }

            auto last = end_node;
            while (const auto node_data = node_datas.find(last))
            {
                const auto from = node_data->from->get_opposite(node_data->node);
                data.moves.add({ from->tile_index, node_data->node->tile_index });
                if (from == start_node) break;
                last = from;
            }
        }
    }
}

Shared<WorldPath> HexaWorld::FindPath(const PathConfig& config)
{
    if (config.from == config.to) return nullptr;
    
    const ChunkIndex chunk_from = config.from.get_chunk();
    const ChunkIndex chunk_to = config.to.get_chunk();

    if ((uint)Math::abs(chunk_to.x - chunk_from.x) <= config.domain_size || (uint)Math::abs(chunk_to.y - chunk_from.y) <= config.domain_size)
    {
        if (!can_step_at(config.to, config.agent_height, this)) return nullptr;
        
        PathFindingData path_finding_data = { config, this, 50.0f };
        build_path(path_finding_data);
        if (path_finding_data.moves.length() == 0) return nullptr;
        List<WorldPath::Segment> moves(path_finding_data.moves.length());
        for (uint i = 0; i < path_finding_data.moves.length(); i++)
        {
            moves[path_finding_data.moves.length() - 1 - i] = path_finding_data.moves[i];
        }
        return MakeShared<WorldPath>(moves);
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

Shared<ItemDrop> HexaWorld::spawn_drop(const TileIndex& tile, const ItemContainer& item, const Vector3& throw_force)
{
    if (item.is_empty()) return nullptr;
    
    Shared<ItemDrop> entity = MakeShared<ItemDrop>(item);
    entity->offset = item.item->mesh->get_bounds_center().z - item.item->mesh->get_bounds_half_size().z;
    //spawn_entity(entity, tile.to_vector() - Vector3(Random::static_number(-0.2f, 0.2f), Random::static_number(-0.2f, 0.2f), entity->offset), Quaternion(Vector3(0, 0, Random::static_number(360.0f))));
    entity->set_force(throw_force);
    return entity;
}

void HexaWorld::set_tile(const TileIndex& world_index, ConstPtr<TileInfo> id) const
{
    const auto chunk_index = world_index.get_chunk();
    if (auto chunk = get_chunk_internal(chunk_index))
    {
        chunk->set_tile(world_index.cycle_chunk(), id);
    }
    else
    {
        if (auto save_game = cast<HexaSaveGame>(Game::get_save_game()))
        {
            if (auto modifications = save_game->get_chunk_modifications(chunk_index))
            {
                modifications->insert(world_index.cycle_chunk(), id);
                save_game->save_chunk_modifications(chunk_index, *modifications);
            }
            else
            {
                Map<TileIndex, ConstPtr<TileInfo>> new_modifications;
                new_modifications.insert(world_index.cycle_chunk(), id);
                save_game->save_chunk_modifications(chunk_index, new_modifications);
            }
        }
    }
}

ConstPtr<TileInfo> HexaWorld::get_tile_id(const TileIndex& world_index) const
{
    const auto chunk_index = world_index.get_chunk();
    if (const auto chunk = get_chunk_internal(chunk_index))
    {
        return chunk->get_tile(world_index.cycle_chunk());
    }

    print_debug("World", "Unable to get tile {$i;$i;$i}, chunk {$i;$i} is not loaded", world_index.x, world_index.y, world_index.z, chunk_index.x, chunk_index.y);
    return nullptr;
}

void HexaWorld::break_tile(const TileIndex& world_index) const
{
    const auto chunk_index = world_index.get_chunk();
    if (auto chunk = get_chunk_internal(chunk_index))
    {
        chunk->break_tile(world_index.cycle_chunk());
    }
}

bool HexaWorld::damage_tile(const TileIndex& world_index, float damage) const
{
    const auto chunk_index = world_index.get_chunk();
    if (auto chunk = get_chunk_internal(chunk_index))
    {
        return chunk->damage_tile(world_index.cycle_chunk(), damage);
    }

    return false;
}

Shared<ComplexTileCustomData> HexaWorld::get_custom_data(const TileIndex& world_index)
{
    const auto chunk_index = world_index.get_chunk();
    if (const auto chunk = get_chunk_internal(chunk_index))
    {
        return chunk->get_custom_data(world_index.cycle_chunk());
    }

    return nullptr;
}

const NavNode* HexaWorld::get_nav_node(const TileIndex& world_index) const
{
    const auto chunk_index = world_index.get_chunk();
    if (const auto chunk = get_chunk_internal(chunk_index))
    {
        return chunk->get_nav_node(world_index);
    }

    return nullptr;
}

SimpleMap<TileIndex, NavConnection*> HexaWorld::get_nav_connections(const TileIndex& world_index) const
{
    const auto chunk_index = world_index.get_chunk();
    if (const auto chunk = get_chunk_internal(chunk_index))
    {
        return chunk->get_nav_connections(world_index.cycle_chunk());
    }

    return {};
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
