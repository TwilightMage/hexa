#pragma once
#include "Engine/Vector3.h"

EXTERN class EXPORT ChunkIndex
{
public:
    ChunkIndex(int x, int y);
    
    bool operator==(const ChunkIndex& rhs) const;
    bool operator!=(const ChunkIndex& rhs) const;

    Vector3 to_vector() const;
    
    int x;
    int y;
};
