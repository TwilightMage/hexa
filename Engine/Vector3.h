#pragma once

#include "framework.h"
#include "Vector2.h"

EXTERN class EXPORT Vector3
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
    
    float x;
    float y;
    float z;

    static Vector3 zero();
    static Vector3 one();
    static Vector3 forward();
    static Vector3 right();
    static Vector3 up();
};
