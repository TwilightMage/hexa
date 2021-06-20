#pragma once

#include "Vector2.h"

class EXPORT Rect
{
public:
    Rect();
    Rect(int x, int y, int w, int h);

    bool intersects(const Rect& rhs) const;
    Shared<Rect> intersection(const Rect& rhs) const;
    FORCEINLINE Vector2 get_position() const;
    FORCEINLINE Vector2 get_size() const;
    FORCEINLINE bool contains(int p_x, int p_y) const;

    FORCEINLINE int left() const;
    FORCEINLINE int right() const;
    FORCEINLINE int top() const;
    FORCEINLINE int bottom() const;

    int x;
    int y;
    int w;
    int h;
};
