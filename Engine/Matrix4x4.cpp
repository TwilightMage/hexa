#include "Matrix4x4.h"

#include <glm/ext/matrix_transform.hpp>

Matrix4x4 Matrix4x4::translate(const Vector3& by) const
{
    Matrix4x4 result = *this;
    result.data[3] = data[0] * by.x + data[1] * by.y + data[2] * by.z + data[3];
    return result;
}

Matrix4x4 Matrix4x4::rotate(const Quaternion& by) const
{
    const auto a = by.axis_angle();
    const auto c = cos(a);
    const auto s = sin(a);

    const auto axis = by.axis().normalized();
    const auto temp = axis * (1 - c);

    Matrix4x4 Rotate;
    Rotate.data[0][0] = c + temp.x * axis.x;
    Rotate.data[0][1] = temp.x * axis.y + s * axis.z;
    Rotate.data[0][2] = temp.x * axis.z - s * axis.y;

    Rotate.data[1][0] = temp.y * axis.x - s * axis.z;
    Rotate.data[1][1] = c + temp.y * axis.y;
    Rotate.data[1][2] = temp.y * axis.z + s * axis.x;

    Rotate.data[2][0] = temp.z * axis.x + s * axis.y;
    Rotate.data[2][1] = temp.z * axis.y - s * axis.x;
    Rotate.data[2][2] = c + temp.z * axis.z;

    Matrix4x4 Result;
    Result.data[0] = data[0] * Rotate.data[0][0] + data[1] * Rotate.data[0][1] + data[2] * Rotate.data[0][2];
    Result.data[1] = data[0] * Rotate.data[1][0] + data[1] * Rotate.data[1][1] + data[2] * Rotate.data[1][2];
    Result.data[2] = data[0] * Rotate.data[2][0] + data[1] * Rotate.data[2][1] + data[2] * Rotate.data[2][2];
    
    Result.data[3] = data[3];
    return Result;
}

Matrix4x4 Matrix4x4::scale(const Vector3& by) const
{
    Matrix4x4 Result;
    Result.data[0] = data[0] * by.x;
    Result.data[1] = data[1] * by.y;
    Result.data[2] = data[2] * by.z;
    Result.data[3] = data[3];
    return Result;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& rhs) const
{
    const Row SrcA0 = data[0];
    const Row SrcA1 = data[1];
    const Row SrcA2 = data[2];
    const Row SrcA3 = data[3];

    const Row SrcB0 = rhs.data[0];
    const Row SrcB1 = rhs.data[1];
    const Row SrcB2 = rhs.data[2];
    const Row SrcB3 = rhs.data[3];

    Matrix4x4 Result;
    Result.data[0] = SrcA0 * SrcB0.x + SrcA1 * SrcB0.y + SrcA2 * SrcB0.z + SrcA3 * SrcB0.w;
    Result.data[1] = SrcA0 * SrcB1.x + SrcA1 * SrcB1.y + SrcA2 * SrcB1.z + SrcA3 * SrcB1.w;
    Result.data[2] = SrcA0 * SrcB2.x + SrcA1 * SrcB2.y + SrcA2 * SrcB2.z + SrcA3 * SrcB2.w;
    Result.data[3] = SrcA0 * SrcB3.x + SrcA1 * SrcB3.y + SrcA2 * SrcB3.z + SrcA3 * SrcB3.w;
    return Result;
}
