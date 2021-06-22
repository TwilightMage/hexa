#pragma once

#include "ChunkIndex.h"
#include "TileSide.h"
#include "Engine/BasicTypes.h"
#include "Engine/Vector3.h"

class EXPORT TileIndex
{
public:
    TileIndex();
    TileIndex(int x, int y, int z);
    
    Vector3 to_vector() const;
    static TileIndex from_vector(const Vector3& vector);

    FORCEINLINE TileIndex offset(int x, int y, int z) const;
    TileIndex offset(TileSide direction) const;
    FORCEINLINE TileIndex cycle_chunk() const;
    FORCEINLINE ChunkIndex get_chunk() const;

    static TileIndex offset_from_side(TileSide side);
    static TileSide offset_to_side(const TileIndex& offset);
    static uint distance_xy(const TileIndex& a, const TileIndex& b);

    TileIndex operator+(const TileIndex& rhs) const;
    TileIndex operator-(const TileIndex& rhs) const;
    TileIndex& operator+=(const TileIndex& rhs);
    TileIndex& operator-=(const TileIndex& rhs);
    
    bool operator<(const TileIndex& rhs) const;
    bool operator==(const TileIndex& rhs) const;
    bool operator!=(const TileIndex& rhs) const;
    
    int x;
    int y;
    int z;
};
