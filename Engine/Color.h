#pragma once

#include "BasicTypes.h"
#include "framework.h"
#include "Quaternion.h"
#include "Vector3.h"

class EXPORT Color
{
public:
    Color();
    Color(const Color& rhs);
    Color(byte r, byte g, byte b);
    Color(byte r, byte g, byte b, byte a);

    FORCEINLINE Vector3 to_vector3() const;
    FORCEINLINE Quaternion to_quaternion() const;
    
    byte r;
    byte g;
    byte b;
    byte a;

    FORCEINLINE static Color white()       { return Color(255, 255, 255, 255); }
    FORCEINLINE static Color grey()        { return Color(127, 127, 127, 255); }
    FORCEINLINE static Color black()       { return Color(0,   0,   0,   255); }
    FORCEINLINE static Color red()         { return Color(255, 0,   0,   255); }
    FORCEINLINE static Color green()       { return Color(0,   255, 0,   255); }
    FORCEINLINE static Color blue()        { return Color(0,   0,   255, 255); }
    FORCEINLINE static Color cyan()        { return Color(0,   255, 255, 255); }
    FORCEINLINE static Color purple()      { return Color(255, 0,   255, 255); }
    FORCEINLINE static Color yellow()      { return Color(255, 255, 0,   255); }
    FORCEINLINE static Color transparent() { return Color(0,   0,   0,   255); }
};
