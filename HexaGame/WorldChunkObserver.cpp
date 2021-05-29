#include "WorldChunkObserver.h"


#include "HexaWorld.h"
#include "WorldChunk.h"

void WorldChunkObserver::set_render_chunks(bool render_chunks)
{
    if (render_chunks_ == render_chunks) return;

    render_chunks_ = render_chunks;

    for (uint i = 1; i < chunks_.get_size_x() - 1; i++)
    {
        for (uint j = 1; j < chunks_.get_size_y() - 1; j++)
        {
            chunks_.at(i, j)->set_visibility(render_chunks_);
        }
    }
}

Rect WorldChunkObserver::get_coverage_rect() const
{
    return Rect(chunk_index_.x - half_size_, chunk_index_.y - half_size_, half_size_ * 2 + 1, half_size_ * 2 + 1);
}

Shared<HexaWorld> WorldChunkObserver::get_world() const
{
    return world_.lock();
}

const ChunkIndex& WorldChunkObserver::get_index() const
{
    return chunk_index_;
}

WorldChunkObserver::WorldChunkObserver(const ChunkIndex& chunk_index, uint half_size, const Weak<HexaWorld>& world)
    : chunk_index_(chunk_index)
    , half_size_(half_size)
    , chunks_(half_size * 2 + 1, half_size * 2 + 1)
    , world_(world)
    , render_chunks_(false)
{
}

void WorldChunkObserver::move(const ChunkIndex& new_chunk_index)
{
    if (auto world = world_.lock())
    {
        world->move_observer(this, new_chunk_index);
    }
}
