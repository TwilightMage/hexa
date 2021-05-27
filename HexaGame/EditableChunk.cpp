#include "EditableChunk.h"

#include "WorldChunkData.h"

const Shared<WorldChunkData>& EditableChunk::get_chunk() const
{
    return chunk_;
}

Shared<const TileInfo>& EditableChunk::tile(const TileIndex& index) const
{
    return chunk_->data[index.x][index.y][index.z];
}

EditableChunk::EditableChunk(const Shared<WorldChunkData>& chunk)
    : chunk_(chunk)
{
}
