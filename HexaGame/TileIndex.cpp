#include "TileIndex.h"

#include "HexaMath.h"
#include "WorldChunk.h"
#include "Engine/Math.h"

TileIndex::TileIndex(int x, int y, uint z)
    : x(x)
    , y(y)
    , z(z)
{
}

Vector3 TileIndex::to_vector() const
{
    return Vector3(
            (Math::sqrt(3.0f) / 2.0f * y + Math::sqrt(3.0f) * -x) * -0.5f,
            (3.0f / 2.0f * y) * 0.5f,
            z / 3.0f
        );
}

TileIndex TileIndex::from_vector(const Vector3& vector)
{
    float x = vector.x / (0.5f * Math::sqrt(3.0f));
    float y = vector.y / (0.5f * Math::sqrt(3.0f));

    const auto temp = Math::floor(x + Math::sqrt(3.0f) * y + 1.0f);

    return TileIndex(Math::floor((Math::floor(2.0f * x + 1.0f) + temp) / 3.0f), Math::floor((temp + Math::floor(-x + Math::sqrt(3.0f) * y + 1.0f)) / 3.0f), static_cast<uint>(vector.z / HexaMath::tile_height));
}

TileIndex TileIndex::offset(int x, int y, int z) const
{
    return TileIndex(this->x + x, this->y + y, this->z + z);
}

TileIndex TileIndex::cycle_chunk() const
{
    return TileIndex(Math::mod(x, WorldChunk::chunk_size), Math::mod(y, WorldChunk::chunk_size), z);
}

ChunkIndex TileIndex::get_chunk() const
{
    return ChunkIndex(x / WorldChunk::chunk_size, y / WorldChunk::chunk_size);
}

bool TileIndex::operator<(const TileIndex& rhs) const
{
    if (x != rhs.x) return x < rhs.x;
    if (y != rhs.y) return y < rhs.y;
    if (z != rhs.z) return z < rhs.z;
    return false;
}

bool TileIndex::operator==(const TileIndex& rhs) const
{
    return x == rhs.x && y == rhs.y && z == rhs.z;
}

bool TileIndex::operator!=(const TileIndex& rhs) const
{
    return x != rhs.x || y != rhs.y || z != rhs.z;
}
