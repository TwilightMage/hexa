﻿#include "Rect.h"

#include "Math.h"

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
    return (value_in_range(x, rhs.x, rhs.x + rhs.w) || value_in_range(rhs.x, x, x + w)) && (value_in_range(y, rhs.y, rhs.y + rhs.h) || value_in_range(rhs.y, y, y + h));
}

Shared<Rect> Rect::intersection(const Rect& rhs) const
{
    int new_x = Math::max(x, rhs.x);
    int new_y = Math::max(y, rhs.y);
    int new_w = Math::min(x + w, rhs.x + rhs.w) - new_x;
    int new_h = Math::min(y + h, rhs.y + rhs.h) - new_y;

    if (new_w > 0 && new_h > 0)
    {
        return MakeShared<Rect>(new_x, new_y, new_w, new_h);
    }

    return nullptr;
}

Vector2 Rect::get_position() const
{
    return Vector2(static_cast<float>(x), static_cast<float>(y));
}

Vector2 Rect::get_size() const
{
    return Vector2(static_cast<float>(w), static_cast<float>(h));
}
