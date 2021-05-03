#pragma once

class Rect
{
public:
    Rect();
    Rect(int x, int y, int w, int h);

    bool intersects(const Rect& rhs) const;
    
    int x;
    int y;
    int w;
    int h;
};
