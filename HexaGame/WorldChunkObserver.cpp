#include "WorldChunkObserver.h"


#include "HexaWorld.h"
#include "WorldChunk.h"

WorldChunkObserver::~WorldChunkObserver()
{
    if (auto world = world_.lock())
    {
        world->unregister_chunk_observer(this);
    }
}

void WorldChunkObserver::set_render_chunks(bool render_chunks)
{
    if (render_chunks_ == render_chunks) return;

    render_chunks_ = render_chunks;

    for (uint i = 1; i < chunks_.get_size_x() - 1; i++)
    {
        for (uint j = 1; j < chunks_.get_size_y() - 1; j++)
        {
            if (render_chunks_)
            {
                chunks_.at(i, j)->inc_visibility();
            }
            else
            {
                chunks_.at(i, j)->dec_visibility();
            }
        }
    }
}

const Rect& WorldChunkObserver::get_rect() const
{
    return rect_;
}

Shared<HexaWorld> WorldChunkObserver::get_world() const
{
    return world_.lock();
}

void WorldChunkObserver::move(const Rect& rect)
{
    if (auto world = world_.lock())
    {
        world->move_observer(this, rect);
    }
}

void WorldChunkObserver::move(const ChunkIndex& new_chunk_index, uint half_size)
{
    move(Rect(new_chunk_index.x - half_size, new_chunk_index.y - half_size, half_size * 2 + 1, half_size * 2 + 1));
}

WorldChunkObserver::WorldChunkObserver(const Rect& rect, const Weak<HexaWorld>& world)
    : rect_(rect)
    , chunks_(rect.w, rect.h)
    , world_(world)
    , render_chunks_(false)
{
}
