#include "ChunkIndex.h"


#include "WorldChunk.h"
#include "Engine/Math.h"

ChunkIndex::ChunkIndex()
    : x(0)
    , y(0)
{
}

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
            (Math::sqrt(3.0f) / 2.0f * y * WorldChunk::chunk_size + Math::sqrt(3.0f) * -x * WorldChunk::chunk_size) * -0.5f,
            (3.0f / 2.0f * y * WorldChunk::chunk_size) * 0.5f,
            0.0f
        );
}

ChunkIndex ChunkIndex::from_vector(const Vector3& vector)
{
    float x = vector.x / (0.5f * Math::sqrt(3.0f));
    float y = vector.y / (0.5f * Math::sqrt(3.0f));

    const auto temp = Math::floor(x + Math::sqrt(3.0f) * y + 1.0f);
    int q = Math::floor((Math::floor(2.0f * x + 1.0f) + temp) / 3.0f);
    int r = Math::floor((temp + Math::floor(-x + Math::sqrt(3.0f) * y + 1.0f)) / 3.0f);

    return ChunkIndex(
        q < 0 ? q / static_cast<int>(WorldChunk::chunk_size) - 1 : q / static_cast<int>(WorldChunk::chunk_size),
        r < 0 ? r / static_cast<int>(WorldChunk::chunk_size) - 1 : r / static_cast<int>(WorldChunk::chunk_size)
        );
}

ChunkIndex ChunkIndex::operator+(const ChunkIndex& rhs) const
{
    return ChunkIndex(x + rhs.x, y + rhs.y);
}

ChunkIndex ChunkIndex::operator-(const ChunkIndex& rhs) const
{
    return ChunkIndex(x - rhs.x, y - rhs.y);
}
