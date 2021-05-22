#pragma once

#include <assert.h>

#include "Quaternion.h"
#include "Vector3.h"

EXTERN class EXPORT Matrix4x4
{
public:
    struct Row
    {
        float x;
        float y;
        float z;
        float w;

        float& operator[](uint index)
        {
            return reinterpret_cast<float*>(this)[index];
        }

        Row operator+(const Row& rhs) const
        {
            return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
        }

        Row operator+(float rhs) const
        {
            return { x + rhs, y + rhs, z + rhs, w + rhs };
        }

        Row operator-(const Row& rhs) const
        {
            return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
        }

        Row operator-(float rhs) const
        {
            return { x - rhs, y - rhs, z - rhs, w - rhs };
        }

        Row operator*(const Row& rhs) const
        {
            return { x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w };
        }

        Row operator*(float rhs) const
        {
            return { x * rhs, y * rhs, z * rhs, w * rhs };
        }

        Row operator/(const Row& rhs) const
        {
            return { x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w };
        }

        Row operator/(float rhs) const
        {
            return { x / rhs, y / rhs, z / rhs, w / rhs };
        }
    };
    
    Row data[4] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}}
    ;

    Matrix4x4 translate(const Vector3& by) const;
    Matrix4x4 rotate(const Quaternion& by) const;
    Matrix4x4 scale(const Vector3& by) const;

    Matrix4x4 operator*(const Matrix4x4& rhs) const;
};
