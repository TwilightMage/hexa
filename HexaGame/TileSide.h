#pragma once

#include "Engine/framework.h"
#include "Engine/Math.h"

enum class TileSide
{
    None        = 0,
    Front       = 1,
    FrontRight  = 2,
    BackRight   = 4,
    Back        = 8,
    BackLeft    = 16,
    FrontLeft   = 32,
    Up          = 64,
    Down        = 128,

    Wall        = Front | FrontRight | BackRight | Back | BackLeft | FrontLeft,
    All         = Up | Down | Wall
};
IMPLEMENT_ENUM_FLAGS(TileSide);

// Unsafe functions, use on your own risk!
// All functions are intended to work with only one side bit!

inline TileSide tile_side_left(TileSide from)
{
    if (from == TileSide::Front) return TileSide::FrontLeft;
    return from >> 1;
}

inline TileSide tile_side_right(TileSide from)
{
    if (from == TileSide::FrontLeft) return TileSide::Front;
    return from << 1;
}

inline TileSide tile_side_from_angle_xy(float global_angle_xy)
{
    float angle = Math::unwind_angle(global_angle_xy);
    if (angle < 0)
    {
        angle += 360;
    }
    return static_cast<TileSide>(1 << Math::round(angle / 60) % 6);
}

inline TileSide tile_side_opposite(TileSide from)
{
    if (const auto from_vert = from & (TileSide::Up | TileSide::Down); !!from_vert)
    {
        return (~from_vert) & (TileSide::Up | TileSide::Down);
    }

    if (from > TileSide::BackRight) return from >> 3;
    return from << 3;
}