#include "HexaWorld.h"

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

Shared<WorldChunkObserver> HexaWorld::register_chunk_observer(const ChunkIndex& chunk_index, uint half_size)
{
    auto observer = MakeSharedInternal(WorldChunkObserver, chunk_index, half_size, cast<HexaWorld>(weak_from_this()));
    Rect observer_rect = observer->get_coverage_rect();
    
    observer_storage_.for_each([&](const point<int, Shared<WorldChunkObserver>>& another_observer)
    {
        Rect another_rect = another_observer.value->get_coverage_rect();

        if (auto intersection = another_rect.intersection(observer_rect))
        {
            for (int x = 0; x < intersection->w; x++)
            {
                for (int y = 0; y < intersection->h; y++)
                {
                    observer->chunks_.at(intersection->x - observer_rect.x + x, intersection->y - observer_rect.y + y) = another_observer.value->chunks_.at(intersection->x - another_rect.x + x, intersection->y - another_rect.y + y);
                }
            }
        }
    });

    fill_observer(observer.get());
    
    observer_storage_.insert({chunk_index.x, chunk_index.y, observer});

    return observer;
}

Shared<WorldChunk> HexaWorld::get_chunk(const ChunkIndex& chunk_index)
{
    std::function callback = [&](const point<int, Shared<WorldChunkObserver>>& observer_point)->bool
    {
        return chunk_index.x >= observer_point.x - observer_point.value->half_size_ && chunk_index.x < observer_point.x + observer_point.value->half_size_ &&
               chunk_index.y >= observer_point.y - observer_point.value->half_size_ && chunk_index.y < observer_point.y + observer_point.value->half_size_;
    };
    if (auto point = observer_storage_.find(callback))
    {
        return point->value->chunks_.at(chunk_index.x - (point->x - point->value->half_size_), chunk_index.y - (point->y - point->value->half_size_));
    }

    return nullptr;
}

void HexaWorld::dump_observable_area()
{
    int min_x = std::numeric_limits<int>::max();
    int min_y = std::numeric_limits<int>::max();
    int max_x = std::numeric_limits<int>::min();
    int max_y = std::numeric_limits<int>::min();
    observer_storage_.for_each([&](const point<int, Shared<WorldChunkObserver>>& observer_point)
        {
            min_x = Math::min(min_x, observer_point.x - observer_point.value->half_size_);
            min_y = Math::min(min_y, observer_point.y - observer_point.value->half_size_);
            max_x = Math::max(max_x, observer_point.x + observer_point.value->half_size_);
            max_y = Math::max(max_y, observer_point.y + observer_point.value->half_size_);
        });

    Array2D<bool> matrix(max_x - min_x, max_y - min_y);

    observer_storage_.for_each([&](const point<int, Shared<WorldChunkObserver>>& observer_point)
        {
            for (uint i = 0; i < observer_point.value->chunks_.get_size_x(); i++)
            {
                for (uint j = 0; j < observer_point.value->chunks_.get_size_y(); j++)
                {
                    auto a = observer_point.value->chunks_.at(j, i) ? true : false;
                    matrix.at(observer_point.x - observer_point.value->half_size_ - min_x + j, observer_point.y - observer_point.value->half_size_ - min_y + i) = a;
                }
            }
        });
    
    List<String> s1;
    for (uint i = 0; i < matrix.get_size_y(); i++)
    {
        List<String> s2;
        for (uint j = 0; j < matrix.get_size_x(); j++)
        {
            if (j + min_x == 0 && i + min_y == 0) s2.Add("()");
            else if (j + min_x == 10 && i + min_y == 10) s2.Add("XX");
            else s2.Add(StringFormat("%s", matrix.at(j, i) ? "00" : "--"));
        }
        s1.Add(StringJoin(s2, " "));
    }
    String s = StringJoin(s1, "\n");
    print_debug("Dump loaded chunks", "From %i %i\n" + s, min_x, min_y);
}

void HexaWorld::move_observer(WorldChunkObserver* observer, const ChunkIndex& to)
{
    const auto old_rect = observer->get_coverage_rect();
    const auto new_rect = Rect(to.x - observer->half_size_, to.y - observer->half_size_, observer->half_size_ * 2 + 1, observer->half_size_ * 2 + 1);

    // Remove retired chunks
    if (const auto intersection = old_rect.intersection(new_rect))
    {
        for (uint x = 0; x < observer->chunks_.get_size_x(); x++)
        {
            for (uint y = 0; y < observer->chunks_.get_size_y(); y++)
            {
                if (!new_rect.contains(old_rect.x + x, old_rect.y + y))
                {
                    auto& chunk = observer->chunks_.at(x, y);
                    if (chunk->get_data()->dec_observe())
                    {
                        chunk->set_visibility(false);
                    }
                    chunk = nullptr;
                }
            }
        }

        // translate matrix cells
        int x_from, x_to, x_dir;
        if (new_rect.x > old_rect.x)
        {
            x_from = 0;
            x_to = intersection->w;
            x_dir = 1;
        }
        else
        {
            x_from = intersection->w - 1;
            x_to = -1;
            x_dir = -1;
        }

        int y_from, y_to, y_dir;
        if (new_rect.x > old_rect.x)
        {
            y_from = 0;
            y_to = intersection->h;
            y_dir = 1;
        }
        else
        {
            y_from = intersection->h - 1;
            y_to = -1;
            y_dir = -1;
        }
        
        Array2D<Shared<WorldChunk>> chunks_buffer = Array2D<Shared<WorldChunk>>(observer->chunks_.get_size_x(), observer->chunks_.get_size_y());
        for (int x = x_from; x != x_to; x += x_dir)
        {
            for (int y = y_from; y != y_to; y += y_dir)
            {
                chunks_buffer.at(intersection->x - new_rect.x + x, intersection->y - new_rect.y + y) = observer->chunks_.at(intersection->x - old_rect.x + x, intersection->y - old_rect.y + y);
            }
        }
        for (int x = 0; x < new_rect.w; x++)
        {
            for (int y = 0; y < new_rect.h; y++)
            {
                if (!old_rect.contains(new_rect.x + x, new_rect.y + y))
                {
                    chunks_buffer.at(x, y) = nullptr;
                }
            }
        }
        observer->chunks_ = chunks_buffer;
    }

    observer_storage_.remove(observer->chunk_index_.x, observer->chunk_index_.y);
    observer->chunk_index_ = to;
    fill_observer(observer);

    if (observer->render_chunks_)
    {
        for (int x = 0; x < new_rect.w; x++)
        {
            for (int y = 0; y < new_rect.h; y++)
            {
                int g_x = new_rect.x + x;
                int g_y = new_rect.y + y;
                
                const bool should_be_rendered = x > 0 && x < new_rect.w - 1 && y > 0 && y < new_rect.h - 1;
                const bool was_rendered = g_x > old_rect.x && g_x < old_rect.x + old_rect.w - 1 && g_y > old_rect.y && g_y < old_rect.y + old_rect.h - 1;
                if (should_be_rendered && !was_rendered)
                {
                    observer->chunks_.at(x, y)->set_visibility(true);
                }
                else if (was_rendered && !should_be_rendered)
                {
                    observer->chunks_.at(x, y)->set_visibility(false);
                }
                    
            }
        }
    }
}

void HexaWorld::fill_observer(WorldChunkObserver* observer)
{
    const auto chunk_index = observer->chunk_index_;
    const auto half_size = observer->half_size_;

    List<ChunkIndex> added;
    
    for (uint x = 0; x < observer->chunks_.get_size_x(); x++)
    {
        for (uint y = 0; y < observer->chunks_.get_size_y(); y++)
        {
            auto& chunk = observer->chunks_.at(x, y);
            if (chunk == nullptr)
            {
                chunk = MakeShared<WorldChunk>(ChunkIndex(chunk_index.x - half_size + x, chunk_index.y - half_size + y), cast<HexaWorld>(shared_from_this()));
                chunk->load();
                added.Add(ChunkIndex(x, y));
            }
        }
    }

    for (auto& index : added)
    {
        const bool _f = index.x < static_cast<int>(observer->chunks_.get_size_x()) - 1; // have local front
        const bool _b = index.x > 0;                                                    // have local back
        const bool _r = index.y < static_cast<int>(observer->chunks_.get_size_y()) - 1; // have local right
        const bool _l = index.y > 0;                                                    // have local left
        const auto chunk_front = _f
            ? observer->chunks_.at(index.x + 1, index.y)
            : get_chunk(ChunkIndex(chunk_index.x + half_size + 1 + 1, chunk_index.y - half_size + index.y));
        const auto chunk_back = _b
            ? observer->chunks_.at(index.x - 1, index.y)
            : get_chunk(ChunkIndex(chunk_index.x - half_size - 1, chunk_index.y - half_size + index.y));
        const auto chunk_right = _r
            ? observer->chunks_.at(index.x, index.y + 1)
            : get_chunk(ChunkIndex(chunk_index.x - half_size + index.x, chunk_index.y + half_size + 1 + 1));
        const auto chunk_left = _l
            ? observer->chunks_.at(index.x, index.y - 1)
            : get_chunk(ChunkIndex(chunk_index.x - half_size + index.x, chunk_index.y - half_size - 1));
        const auto chunk_front_right = _f
            ? _r
                ? observer->chunks_.at(index.x + 1, index.y + 1)
                : get_chunk(ChunkIndex(chunk_index.x - half_size + index.x + 1, chunk_index.y + half_size + 1 + 1))
            : _r
                ? get_chunk(ChunkIndex(chunk_index.x + half_size + 1 + 1, chunk_index.y - half_size + index.y + 1))
                : get_chunk(ChunkIndex(chunk_index.x + half_size + 1 + 1, chunk_index.y + half_size + 1 + 1));
        const auto chunk_back_left = _b
            ? _l
                ? observer->chunks_.at(index.x - 1, index.y - 1)
                : get_chunk(ChunkIndex(chunk_index.x - half_size + index.x - 1, chunk_index.y - half_size - 1))
            : _l
                ? get_chunk(ChunkIndex(chunk_index.x - half_size - 1, chunk_index.y - half_size + index.y - 1))
                : get_chunk(ChunkIndex(chunk_index.x - half_size - 1, chunk_index.y - half_size - 1));
        auto& chunk = observer->chunks_.at(index.x, index.y);
        chunk->get_data()->inc_observe();
        chunk->get_data()->link(
            chunk_front ? chunk_front->get_data().get() : nullptr,
            chunk_right ? chunk_right->get_data().get() : nullptr,
            chunk_front_right ? chunk_front_right->get_data().get() : nullptr,
            chunk_back ? chunk_back->get_data().get() : nullptr,
            chunk_left ? chunk_left->get_data().get() : nullptr,
            chunk_back_left ? chunk_back_left->get_data().get() : nullptr
            );
    }
}
