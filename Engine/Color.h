#pragma once

#include "BasicTypes.h"

class Color
{
public:
    Color();
    Color(const Color& rhs);
    Color(byte r, byte g, byte b);
    Color(byte r, byte g, byte b, byte a);
    
    byte r;
    byte g;
    byte b;
    byte a;
};
