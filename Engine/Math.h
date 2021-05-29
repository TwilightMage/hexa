#pragma once

#include <valarray>

#include "framework.h"

#undef min
#undef max

class Math
{
public:
    template<typename T>
    static T pi()
    {
        return static_cast<T>(3.141592653589793238462643383279502884197169399375105820974944592);
    }

    template<typename T>
    static T epsilon()
    {
        return static_cast<T>(2.2204460492503131e-016);
    }

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

    FORCEINLINE static uint mod(int value, uint m)
    {
        int mod = value % (int)m;
        if (mod < 0) {
            mod += m;
        }
        return mod;
    }

    template<typename T>
    static T degrees(T radians)
    {
        return radians / pi<T>() * static_cast<T>(180);
    }

    template<typename T>
    static T radians(T degrees)
    {
        return degrees / static_cast<T>(180) * pi<T>();
    }
    
    template<typename T>
    static T sqr(T in)
    {
        return in * in;
    }

    template<typename T>
    static T pow(T in, T pow)
    {
        return std::pow(in, pow);
    }
    
    template<typename T>
    static T sqrt(T in)
    {
        return std::sqrt(in);
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

    template<typename T>
    static int floor(T in)
    {
        return static_cast<int>(::floor(in));
    }

    template<typename T>
    static int ceil(T in)
    {
        return static_cast<int>(::ceil(in));
    }

    template<typename T>
    static int round(T in)
    {
        return static_cast<int>(::round(in));
    }
};

