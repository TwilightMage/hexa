#pragma once

#include "framework.h"
#include "Math.h"
#include "String.h"
#include "Vector2.h"

class EXPORT Vector3
{
public:
    Vector3();
    explicit Vector3(float f);
    Vector3(float x, float y);
    Vector3(float x, float y, float z);
    Vector3(const Vector2& rhs);
    Vector3(const Vector3& rhs);

    float magnitude() const;
    float dot_product(const Vector3& rhs) const;
    Vector3 cross_product(const Vector3& rhs) const;
    Vector3 normalized() const;
    void normalize();
    String to_string() const;

    FORCEINLINE static float angle_deg(const Vector3& a, const Vector3& b) { return Math::acos_deg(a.normalized().dot_product(b.normalized())); }
    FORCEINLINE static float angle_rad(const Vector3& a, const Vector3& b) { return Math::acos_rad(a.normalized().dot_product(b.normalized())); }
    FORCEINLINE static float distance(const Vector3& a, const Vector3& b) { return (a - b).magnitude(); }
    
    Vector3 operator+(const Vector3& rhs) const;
    Vector3 operator-(const Vector3& rhs) const;
    Vector3 operator*(const Vector3& rhs) const;
    Vector3 operator/(const Vector3& rhs) const;
    void operator+=(const Vector3& rhs);
    void operator-=(const Vector3& rhs);
    void operator*=(const Vector3& rhs);
    void operator/=(const Vector3& rhs);
    Vector3 operator+(float rhs) const;
    Vector3 operator-(float rhs) const;
    Vector3 operator*(float rhs) const;
    Vector3 operator/(float rhs) const;
    void operator+=(float rhs);
    void operator-=(float rhs);
    void operator*=(float rhs);
    void operator/=(float rhs);
    bool operator==(const Vector3& rhs) const;
    bool operator!=(const Vector3& rhs) const;

    Vector3 operator-() const;

    float sum_all() const;
    
    float x;
    float y;
    float z;

    static Vector3 zero();
    static Vector3 one();
    static Vector3 forward();
    static Vector3 right();
    static Vector3 up();
};
