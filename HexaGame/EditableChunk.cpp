#include "EditableChunk.h"

#include "WorldChunk.h"

const Shared<WorldChunk>& EditableChunk::get_chunk() const
{
    return chunk_;
}

Shared<const TileInfo>& EditableChunk::tile(const TileIndex& index) const
{
    return chunk_->data[index.x][index.y][index.z];
}

EditableChunk::EditableChunk(const Shared<WorldChunk>& chunk)
    : chunk_(chunk)
{
}
