#pragma once

#include "Engine/BasicTypes.h"
#include "Engine/Vector3.h"

class TileIndex
{
public:
    TileIndex(int x, int y, uint z);
    
    Vector3 to_vector() const;
    static TileIndex from_index(const Vector3& vector);

    TileIndex offset(int x, int y, int z) const;
    TileIndex cycle_chunk() const;
    
    int x;
    int y;
    uint z;
};
