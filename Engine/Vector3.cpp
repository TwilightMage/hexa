#include "Vector3.h"

#include "Math.h"

Vector3::Vector3()
    : x(0.0f)
    , y(0.0f)
    , z(0.0f)
{
}

Vector3::Vector3(float f)
    : x(f)
    , y(f)
    , z(f)
{
}

Vector3::Vector3(float x, float y)
    : x(x)
    , y(y)
    , z(0.0f)
{
}

Vector3::Vector3(float x, float y, float z)
    : x(x)
    , y(y)
    , z(z)
{
}

Vector3::Vector3(const Vector2& rhs)
    : x(rhs.x)
    , y(rhs.y)
    , z(0.0f)
{
}

Vector3::Vector3(const Vector3& rhs)
    : x(rhs.x)
    , y(rhs.y)
    , z(rhs.z)
{
}

float Vector3::magnitude() const
{
    return Math::sqrt(x * x + y * y + z * z);
}

float Vector3::dot_product(const Vector3& rhs) const
{
    return x * rhs.x + y * rhs.y + z * rhs.z;
}

Vector3 Vector3::cross_product(const Vector3& rhs) const
{
    return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
}

Vector3 Vector3::normalized() const
{
    const auto m = magnitude();
    return Vector3(x / m, y / m, z / m);
}

void Vector3::normalize()
{
    const auto m = magnitude();
    x /= m;
    y /= m;
    z /= m;
}

String Vector3::to_string() const
{
    return String::format("{ x=%f, y=%f, z=%f}", x, y, z);
}

float Vector3::angle_deg(const Vector3& a, const Vector3& b)
{
    return Math::acos_deg(a.normalized().dot_product(b.normalized()));
}

float Vector3::angle_rad(const Vector3& a, const Vector3& b)
{
    return Math::acos_rad(a.normalized().dot_product(b.normalized()));
}

float Vector3::distance(const Vector3& a, const Vector3& b)
{
    return (a - b).magnitude();
}

Vector3 Vector3::operator+(const Vector3& rhs) const
{
    return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vector3 Vector3::operator-(const Vector3& rhs) const
{
    return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vector3 Vector3::operator*(const Vector3& rhs) const
{
    return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
}

Vector3 Vector3::operator/(const Vector3& rhs) const
{
    return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
}

void Vector3::operator+=(const Vector3& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
}

void Vector3::operator-=(const Vector3& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
}

void Vector3::operator*=(const Vector3& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
}

void Vector3::operator/=(const Vector3& rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
}

Vector3 Vector3::operator+(float rhs) const
{
    return Vector3(x + rhs, y + rhs, z + rhs);
}

Vector3 Vector3::operator-(float rhs) const
{
    return Vector3(x - rhs, y - rhs, z - rhs);
}

Vector3 Vector3::operator*(float rhs) const
{
    return Vector3(x * rhs, y * rhs, z * rhs);
}

Vector3 Vector3::operator/(float rhs) const
{
    return Vector3(x / rhs, y / rhs, z / rhs);
}

void Vector3::operator+=(float rhs)
{
    x += rhs;
    y += rhs;
    z += rhs;
}

void Vector3::operator-=(float rhs)
{
    x -= rhs;
    y -= rhs;
    z -= rhs;
}

void Vector3::operator*=(float rhs)
{
    x *= rhs;
    y *= rhs;
    z *= rhs;
}

void Vector3::operator/=(float rhs)
{
    x /= rhs;
    y /= rhs;
    z /= rhs;
}

bool Vector3::operator==(const Vector3& rhs) const
{
    return (x == rhs.x && y == rhs.y && z == rhs.z);
}

bool Vector3::operator!=(const Vector3& rhs) const
{
    return (x != rhs.x || y != rhs.y || z != rhs.z);
}

Vector3 Vector3::operator-() const
{
    return Vector3(-x, -y, -z);
}

float Vector3::sum_all() const
{
    return x + y + z;
}

Vector3 Vector3::zero()
{
    return Vector3(0.0f, 0.0f, 0.0f);
}

Vector3 Vector3::one()
{
    return Vector3(1.0f, 1.0f, 1.0f);
}

Vector3 Vector3::forward()
{
    return Vector3(1.0f, 0.0f, 0.0f);
}

Vector3 Vector3::right()
{
    return Vector3(0.0f, 1.0f, 0.0f);
}

Vector3 Vector3::up()
{
    return Vector3(0.0f, 0.0f, 1.0f);
}
