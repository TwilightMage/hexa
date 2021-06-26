#pragma once

#include "Engine/framework.h"

enum class TileType
{
    None    = 0,
    Air     = 1,
    Liquid  = 2,
    Complex = 4,
    Solid   = 8
};
IMPLEMENT_ENUM_FLAGS(TileType)