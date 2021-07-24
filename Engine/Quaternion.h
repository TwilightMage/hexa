#pragma once

#include "Vector3.h"

class EXPORT Quaternion
{
public:
    Quaternion();
    Quaternion(float x, float y, float z, float w);
    Quaternion(const Quaternion& rhs);
    Quaternion(const Vector3& euler);

    FORCEINLINE float magnitude() const;
    Vector3 rotate_vector3(const Vector3& rhs) const;
    Vector3 forward() const;
    Vector3 right() const;
    Vector3 up() const;
    float pitch() const;
    float yaw() const;
    float roll() const;
    Vector3 euler() const;
    Quaternion normalized() const;
    void normalize();
    String to_string() const;
    Quaternion conjugate() const;
    Vector3 axis() const;
    float axis_angle() const;
    float dot(const Quaternion& rhs) const;

    static Quaternion from_axis_angle(const Vector3& axis, float angle);
    FORCEINLINE static Quaternion look_at(const Vector3& from, const Vector3& to) { return look_at((to - from).normalized()); }
    static Quaternion look_at(const Vector3& normal);
    static Quaternion slerp(const Quaternion& a, const Quaternion& b, float alpha);
    static Quaternion lerp(const Quaternion& from, const Quaternion& to, float alpha);
    
    Quaternion rotate_around(const Vector3& axis, float angle) const;
    Quaternion rotate_around_x(float angle) const;
    Quaternion rotate_around_y(float angle) const;
    Quaternion rotate_around_z(float angle) const;

    Quaternion operator*(const Quaternion& rhs) const;
    Quaternion& operator*=(const Quaternion& rhs);
    bool operator==(const Quaternion& rhs) const;
    bool operator!=(const Quaternion& rhs) const;
    Quaternion operator-() const;
    
    float x;
    float y;
    float z;
    float w;
};
