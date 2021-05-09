#pragma once

#include "framework.h"

EXTERN class EXPORT Margins
{
public:
    Margins();
    Margins(int left, int right, int top, int bottom);
    
    int left;
    int right;
    int top;
    int bottom;
};
