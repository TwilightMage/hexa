#pragma once

#include "framework.h"
#include "ui/TextBlock.h"

class Math
{
public:    
    inline const static float pi = 3.141592653589793238462643383279502884197169399375105820974944592f;
    inline const static double pi_double = 3.141592653589793238462643383279502884197169399375105820974944592f;

    template<typename T>
    static T min(T a, T b)
    {
        return a < b ? a : b;
    }

    template<typename T>
    static T max(T a, T b)
    {
        return a > b ? a : b;
    }

    static float degrees(float radians)
    {
        return radians / pi * 180;
    }

    static double degrees(double radians)
    {
        return radians / pi_double * 180;
    }

    static float radians(float degrees)
    {
        return degrees / 180 * pi;
    }

    static double radians(double degrees)
    {
        return degrees / 180 * pi_double;
    }

    template<typename T>
    static T sin_deg(T deg)
    {
        return sin(radians(deg));
    }

    template<typename T>
    static T sin_rad(T rad)
    {
        return sin(rad);
    }

    template<typename T>
    static T cos_deg(T deg)
    {
        return cos(radians(deg));
    }

    template<typename T>
    static T cos_rad(T rad)
    {
        return cos(rad);
    }
};

