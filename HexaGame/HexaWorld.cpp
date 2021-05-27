#include "HexaWorld.h"

#include "Engine/Rect.h"
#include "WorldChunk.h"
#include "WorldGenerator.h"
#include "Engine/Math.h"

void HexaWorld::ChunkObserver::set_render_chunks(bool render_chunks)
{
    if (render_chunks_ == render_chunks) return;

    render_chunks_ = render_chunks;

    for (uint i = 0; i < chunks_.get_size_x(); i++)
    {
        for (uint j = 0; j < chunks_.get_size_y(); j++)
        {
            auto a = chunks_.at(i, j);
            a->set_visibility(render_chunks_);
        }
    }
}

Rect HexaWorld::ChunkObserver::get_coverage_rect() const
{
    return Rect(chunk_index_.x - half_size_, chunk_index_.y - half_size_, half_size_ * 2 + 1, half_size_ * 2 + 1);
}

HexaWorld::ChunkObserver::ChunkObserver(const ChunkIndex& chunk_index, uint half_size, const Weak<HexaWorld>& world)
    : chunk_index_(chunk_index)
    , half_size_(half_size)
    , chunks_(half_size * 2 + 1, half_size * 2 + 1)
    , world_(world)
    , render_chunks_(false)
{
}

HexaWorld::HexaWorld(const Shared<WorldGenerator>& generator)
    : World()
    , generator_(generator)
{

}

const Shared<WorldGenerator>& HexaWorld::get_generator() const
{
    return generator_;
}

Shared<HexaWorld::ChunkObserver> HexaWorld::register_chunk_observer(const ChunkIndex& chunk_index, uint half_size)
{
    auto observer = MakeSharedInternal(ChunkObserver, chunk_index, half_size, cast<HexaWorld>(weak_from_this()));
    Rect observer_rect = observer->get_coverage_rect();
    
    chunk_storage_.for_each([&](const point<int, Shared<ChunkObserver>>& observer_point)
    {
        Rect rect = observer_point.value->get_coverage_rect();

        if (auto intersection = rect.intersection(observer_rect))
        {
            for (int x = 0; x < intersection->w; x++)
            {
                for (int y = 0; y < intersection->h; y++)
                {
                    observer->chunks_.at(intersection->x - observer_rect.x + x, intersection->y - observer_rect.y + y) = observer_point.value->chunks_.at(intersection->x - rect.x + x, intersection->y - rect.y + y);
                }
            }
        }
    });

    for (uint x = 0; x < observer->chunks_.get_size_x(); x++)
    {
        for (uint y = 0; y < observer->chunks_.get_size_y(); y++)
        {
            auto& chunk = observer->chunks_.at(x, y);
            if (chunk == nullptr)
            {
                chunk = MakeShared<WorldChunk>(ChunkIndex(chunk_index.x - half_size + x, chunk_index.y - half_size + y), cast<HexaWorld>(shared_from_this()));
                chunk->load();
            }
        }
    }
    
    chunk_storage_.insert({chunk_index.x, chunk_index.y, observer});

    return observer;
}

Shared<WorldChunk> HexaWorld::get_chunk(const ChunkIndex& chunk_index)
{
    std::function callback = [&](const point<int, Shared<ChunkObserver>>& observer_point)->bool
    {
        return chunk_index.x >= observer_point.x - observer_point.value->half_size_ && chunk_index.x < observer_point.x + observer_point.value->half_size_ &&
               chunk_index.y >= observer_point.y - observer_point.value->half_size_ && chunk_index.y < observer_point.y + observer_point.value->half_size_;
    };
    if (auto point = chunk_storage_.find(callback))
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
    chunk_storage_.for_each([&](const point<int, Shared<ChunkObserver>>& observer_point)
        {
            min_x = Math::min(min_x, observer_point.x - observer_point.value->half_size_);
            min_y = Math::min(min_y, observer_point.y - observer_point.value->half_size_);
            max_x = Math::max(max_x, observer_point.x + observer_point.value->half_size_);
            max_y = Math::max(max_y, observer_point.y + observer_point.value->half_size_);
        });

    Array2D<bool> matrix(max_x - min_x, max_y - min_y);

    chunk_storage_.for_each([&](const point<int, Shared<ChunkObserver>>& observer_point)
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

Shared<WorldChunk> HexaWorld::generate_chunk_callback(const ChunkIndex& tile_index)
{
    return nullptr;
}

void HexaWorld::move_observer(ChunkObserver* observer, const ChunkIndex& to)
{
    
}
