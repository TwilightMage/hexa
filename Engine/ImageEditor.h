#pragma once

#include "BasicTypes.h"
#include "framework.h"

class Color;

class EXPORT ImageEditor
{
public:
    static void copy_rect(const Color* src, uint src_x, uint src_y, uint src_width, Color* dst, uint dst_x, uint dst_y, uint dst_width, uint rgn_width, uint rgn_height);
};
