#pragma once

#include "framework.h"

class Vector3;

EXTERN class EXPORT Vector2
{
public:
    Vector2();
    explicit Vector2(float f);
    Vector2(float x, float y);
    Vector2(const Vector3& rhs);
    Vector2(const Vector2& rhs);

    float magnitude() const;
    float dot_product(const Vector2& rhs) const;
    Vector2 normalized() const;
    void normalize();

    Vector2 operator+(const Vector2& rhs) const;
    Vector2 operator-(const Vector2& rhs) const;
    Vector2 operator*(const Vector2& rhs) const;
    Vector2 operator/(const Vector2& rhs) const;
    void operator+=(const Vector2& rhs);
    void operator-=(const Vector2& rhs);
    void operator*=(const Vector2& rhs);
    void operator/=(const Vector2& rhs);
    Vector2 operator+(float rhs) const;
    Vector2 operator-(float rhs) const;
    Vector2 operator*(float rhs) const;
    Vector2 operator/(float rhs) const;
    void operator+=(float rhs);
    void operator-=(float rhs);
    void operator*=(float rhs);
    void operator/=(float rhs);
    bool operator==(const Vector2& rhs) const;
    bool operator!=(const Vector2& rhs) const;
    
    float x;
    float y;

    static Vector2 zero();
    static Vector2 one();
    static Vector2 right();
    static Vector2 up();
};
