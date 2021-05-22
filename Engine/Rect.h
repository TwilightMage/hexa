#pragma once
#include "Vector2.h"

class Rect
{
public:
    Rect();
    Rect(int x, int y, int w, int h);

    bool intersects(const Rect& rhs) const;
    Vector2 get_position() const;
    Vector2 get_size() const;
    
    int x;
    int y;
    int w;
    int h;
};
