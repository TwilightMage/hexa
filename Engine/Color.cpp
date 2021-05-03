#include "Color.h"

Color::Color()
    : r(0)
    , g(0)
    , b(0)
    , a(255)
{
}

Color::Color(const Color& rhs)
    : r(rhs.r)
    , g(rhs.g)
    , b(rhs.b)
    , a(rhs.a)
{
}

Color::Color(byte r, byte g, byte b)
    : r(r)
    , g(g)
    , b(b)
    , a(255)
{
}

Color::Color(byte r, byte g, byte b, byte a)
    : r(r)
    , g(g)
    , b(b)
    , a(a)
{
}
