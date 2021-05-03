#include "Rect.h"

Rect::Rect()
    : x(0)
    , y(0)
    , w(0)
    , h(0)
{
}

Rect::Rect(int x, int y, int w, int h)
    : x(x)
    , y(y)
    , w(w)
    , h(h)
{
}

bool value_in_range(int value, int min, int max) { return (value >= min) && (value < max); }

bool Rect::intersects(const Rect& rhs) const
{
    auto a = value_in_range(x, rhs.x, rhs.x + rhs.w) || value_in_range(rhs.x, x, x + w);
    auto b = value_in_range(y, rhs.y, rhs.y + rhs.h) || value_in_range(rhs.y, y, y + h);
    return a && b;
}
