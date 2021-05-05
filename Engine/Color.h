#pragma once

#include "BasicTypes.h"
#include "framework.h"
#include "Vector3.h"

EXTERN class EXPORT Color
{
public:
    Color();
    Color(const Color& rhs);
    Color(byte r, byte g, byte b);
    Color(byte r, byte g, byte b, byte a);

    Vector3 to_vector3() const;
    
    byte r;
    byte g;
    byte b;
    byte a;

    static Color white;
};
