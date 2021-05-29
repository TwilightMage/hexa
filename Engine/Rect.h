#pragma once

#include "Vector2.h"

EXTERN class EXPORT Rect
{
public:
    Rect();
    Rect(int x, int y, int w, int h);

    bool intersects(const Rect& rhs) const;
    Shared<Rect> intersection(const Rect& rhs) const;
    Vector2 get_position() const;
    Vector2 get_size() const;
    bool contains(int p_x, int p_y) const;

    int x;
    int y;
    int w;
    int h;
};
