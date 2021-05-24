#pragma once

#include "Engine/framework.h"

enum class TileSide
{
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
