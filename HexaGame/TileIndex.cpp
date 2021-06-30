#include "TileIndex.h"

#include "HexaMath.h"
#include "WorldChunk.h"
#include "Engine/Math.h"

TileIndex::TileIndex()
    : x(0)
    , y(0)
    , z(0)
{
}

TileIndex::TileIndex(int x, int y, int z)
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

TileIndex TileIndex::offset(TileSide direction) const
{
    return *this + offset_from_side(direction);
}

TileIndex TileIndex::cycle_chunk() const
{
    return TileIndex(Math::mod(x, WorldChunk::chunk_size), Math::mod(y, WorldChunk::chunk_size), z);
}

ChunkIndex TileIndex::get_chunk() const
{
    return ChunkIndex((x - static_cast<int>(Math::mod(x, WorldChunk::chunk_size))) / static_cast<int>(WorldChunk::chunk_size), (y - static_cast<int>(Math::mod(y, WorldChunk::chunk_size))) / static_cast<int>(WorldChunk::chunk_size));
}

TileIndex TileIndex::to_absolute(const ChunkIndex& parent_chunk) const
{
    return TileIndex(parent_chunk.x * WorldChunk::chunk_size + x, parent_chunk.y * WorldChunk::chunk_size + y, z);
    
}

bool TileIndex::is_inside_chunk() const
{
    return x >= 0 && x < WorldChunk::chunk_size && y >= 0 && y < WorldChunk::chunk_size && z >= 0 && z < WorldChunk::chunk_height;
}

TileIndex TileIndex::offset_from_side(TileSide side)
{
    switch (side)
    {
    case TileSide::Front:
        return TileIndex(1, 0, 0);
    case TileSide::FrontRight:
        return TileIndex(1, 1, 0);
    case TileSide::BackRight:
        return TileIndex(0, 1, 0);
    case TileSide::Back:
        return TileIndex(-1, 0, 0);
    case TileSide::BackLeft:
        return TileIndex(-1, -1, 0);
    case TileSide::FrontLeft:
        return TileIndex(0, -1, 0);
    case TileSide::Up:
        return TileIndex(0, 0, 1);
    case TileSide::Down:
        return TileIndex(0, 0, -1);
    default:
        return TileIndex(0, 0, 0);
    }
}

TileSide TileIndex::offset_to_side(const TileIndex& offset)
{
    if (offset == TileIndex( 1,  0,  0)) return TileSide::Front;
    if (offset == TileIndex( 1,  1,  0)) return TileSide::FrontRight;
    if (offset == TileIndex( 0,  1,  0)) return TileSide::BackRight;
    if (offset == TileIndex(-1,  0,  0)) return TileSide::Back;
    if (offset == TileIndex(-1, -1,  0)) return TileSide::BackLeft;
    if (offset == TileIndex( 0, -1,  0)) return TileSide::FrontLeft;
    if (offset == TileIndex( 0,  0,  1)) return TileSide::Up;
    if (offset == TileIndex( 0,  0, -1)) return TileSide::Down;

    return TileSide::None;
}

uint TileIndex::distance_xy(const TileIndex& a, const TileIndex& b)
{
    return (Math::abs(a.x - b.x) + Math::abs(a.x + b.y - a.y - b.x) + Math::abs(a.y - b.y)) / 2;
}

TileIndex TileIndex::operator+(const TileIndex& rhs) const
{
    return TileIndex(x + rhs.x, y + rhs.y, z + rhs.z);
}

TileIndex TileIndex::operator-(const TileIndex& rhs) const
{
    return TileIndex(x - rhs.x, y - rhs.y, z - rhs.z);
}

TileIndex& TileIndex::operator+=(const TileIndex& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;

    return *this;
}

TileIndex& TileIndex::operator-=(const TileIndex& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;

    return *this;
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
