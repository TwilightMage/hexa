#include "ChunkIndex.h"


#include "WorldChunkData.h"
#include "Engine/Math.h"

ChunkIndex::ChunkIndex(int x, int y)
    : x(x)
    , y(y)
{
}

bool ChunkIndex::operator==(const ChunkIndex& rhs) const
{
    return x == rhs.x && y == rhs.y;
}

bool ChunkIndex::operator!=(const ChunkIndex& rhs) const
{
    return x != rhs.x || y != rhs.y;
}

Vector3 ChunkIndex::to_vector() const
{
    return Vector3(
            (Math::sqrt(3.0f) / 2.0f * y * WorldChunkData::chunk_size + Math::sqrt(3.0f) * -x * WorldChunkData::chunk_size) * -0.5f,
            (3.0f / 2.0f * y * WorldChunkData::chunk_size) * 0.5f,
            0.0f
        );
}
