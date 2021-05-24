#include "TileIndex.h"

#include "HexaMath.h"
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

TileIndex TileIndex::from_index(const Vector3& vector)
{
    float x = vector.x / (0.5f * Math::sqrt(3.0f));
    float y = vector.y / (0.5f * Math::sqrt(3.0f));

    const auto temp = Math::floor(x + Math::sqrt(3.0f) * y + 1.0f);
    int q = Math::floor((Math::floor(2.0f * x + 1.0f) + temp) / 3.0f);
    int r = Math::floor((temp + Math::floor(-x + Math::sqrt(3.0f) * y + 1.0f)) / 3.0f);

    return TileIndex(Math::floor((Math::floor(2.0f * x + 1.0f) + temp) / 3.0f), Math::floor((temp + Math::floor(-x + Math::sqrt(3.0f) * y + 1.0f)) / 3.0f), static_cast<uint>(vector.z / HexaMath::tile_height));
}
