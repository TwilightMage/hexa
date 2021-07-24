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

Shared<const TileInfo>& EditableChunk::tile(int x, int y, int z) const
{
    return chunk_->data[x][y][z];
}

void EditableChunk::set_complex_custom_data(const TileIndex& index, const Shared<ComplexTileCustomData>& custom_data) const
{
    chunk_->custom_data_[index] = custom_data;
}

EditableChunk::EditableChunk(const Shared<WorldChunk>& chunk)
    : chunk_(chunk)
{
}
