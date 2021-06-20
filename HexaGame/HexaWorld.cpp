#include "HexaWorld.h"

#include "Character.h"
#include "HexaSaveGame.h"
#include "Engine/Rect.h"
#include "WorldChunk.h"
#include "WorldChunkObserver.h"
#include "WorldGenerator.h"
#include "Engine/Math.h"

HexaWorld::HexaWorld(const Shared<WorldGenerator>& generator)
    : World()
    , generator_(generator)
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
    
    chunk_observers_.Add(observer);

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

bool HexaWorld::spawn_character(const Shared<Character>& character, const TileIndex& tile_index)
{
    if (characters_.Contains(character)) return false;
    
    for (auto charact : characters_)
    {
        if (charact->get_tile_position() == tile_index) return false;
    }

    spawn_entity(character);

    characters_.Add(character);
    character->set_tile_position(tile_index);

    character->on_destroyed.bind(this, &HexaWorld::character_destroyed_callback);

    return true;
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
            if (j + min_x == 0 && i + min_y == 0) s2.Add("()");
            else if (j + min_x == 10 && i + min_y == 10) s2.Add("XX");
            else s2.Add(String::format("%s", matrix.at(j, i) ? "00" : "--"));
        }
        s1.Add(String::join(s2, " "));
    }
    String s = String::join(s1, "\n");
    print_debug("Dump loaded chunks", "From %i %i\n" + s, min_x, min_y);
}

void HexaWorld::character_destroyed_callback(const Shared<Entity>& character)
{
    character->on_destroyed.unbind(this, &HexaWorld::character_destroyed_callback);
    
    if (auto charact = cast<Character>(character))
    {
        characters_.Remove(charact);
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

    chunk_observers_.RemoveAt(index);
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
                chunk->load();
                added.Add(ChunkIndex(x, y));
            }
        }
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
