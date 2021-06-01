﻿#pragma once

#include "Engine/BasicTypes.h"
#include "Engine/Vector3.h"

EXTERN class EXPORT TileIndex
{
public:
    TileIndex(int x, int y, uint z);
    
    Vector3 to_vector() const;
    static TileIndex from_vector(const Vector3& vector);

    TileIndex offset(int x, int y, int z) const;
    TileIndex cycle_chunk() const;

    bool operator<(const TileIndex& rhs) const;
    bool operator==(const TileIndex& rhs) const;
    bool operator!=(const TileIndex& rhs) const;
    
    int x;
    int y;
    uint z;
};