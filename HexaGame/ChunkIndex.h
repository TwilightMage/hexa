#pragma once
#include "Engine/Vector3.h"

EXTERN class EXPORT ChunkIndex
{
public:
    ChunkIndex();
    ChunkIndex(int x, int y);
    
    bool operator==(const ChunkIndex& rhs) const;
    bool operator!=(const ChunkIndex& rhs) const;

    Vector3 to_vector() const;
    static ChunkIndex from_vector(const Vector3& vector);
    ChunkIndex operator+(const ChunkIndex& rhs) const;
    ChunkIndex operator-(const ChunkIndex& rhs) const;
    
    int x;
    int y;
};
