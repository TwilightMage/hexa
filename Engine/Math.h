#pragma once

#include "framework.h"
#include "ui/TextBlock.h"

class Math
{
public:    
    inline const static float pi = 3.141592653589793238462643383279502884197169399375105820974944592f;

    template<typename T>
    static T min(const T& a, const T& b)
    {
        return a < b ? a : b;
    }

    template<typename T>
    static T max(const T& a, const T& b)
    {
        return a > b ? a : b;
    }
};

