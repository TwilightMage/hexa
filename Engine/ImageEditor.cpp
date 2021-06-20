#include "ImageEditor.h"

#include "Color.h"

void ImageEditor::copy_rect(const Color* src, uint src_x, uint src_y, uint src_width, Color* dst, uint dst_x, uint dst_y, uint dst_width, uint rgn_width, uint rgn_height)
{
    for (uint y = 0; y < rgn_height; y++)
    {
        const auto src_offset = src_width * (src_y + y) + src_x;
        const auto dst_offset = dst_width * (dst_y + y) + dst_x;
        memcpy(dst + dst_offset, src + src_offset, sizeof(Color) * rgn_width);
    }
}
