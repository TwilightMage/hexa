#pragma once
#include "TileIndex.h"
#include "Engine/Delegate.h"

struct PathConfig
{
    uint domain_size = 3;
    TileIndex from;
    TileIndex to;
    uint agent_height;

    uint jump_distance = 1;
    uint jump_height = 1;
    uint allowed_fall = 2;
    uint allowed_climb = 1;
    bool can_fly = false;
};
