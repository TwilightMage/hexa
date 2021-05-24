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

    Matrix4x4();
    explicit Matrix4x4(float scale);
    Matrix4x4(float x0, float y0, float z0, float w0, float x1, float y1, float z1, float w1, float x2, float y2, float z2, float w2, float x3, float y3, float z3, float w3);
    
    Row data[4];

    Matrix4x4 translate(const Vector3& by) const;
    Matrix4x4 rotate(const Quaternion& by) const;
    Matrix4x4 scale(const Vector3& by) const;

    static Matrix4x4 look_at(const Vector3& from, const Vector3& to);
    static Matrix4x4 perspective(float fov_y, float aspect, float z_near, float z_far);
    static Matrix4x4 ortho(float left, float right, float bottom, float top, float z_near, float z_far);

    Matrix4x4 operator*(const Matrix4x4& rhs) const;
};
