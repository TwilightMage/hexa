#pragma once
#include "TileIndex.h"
#include "Engine/Delegate.h"

struct PathConfig
{
    uint domain_size = 3;
    TileIndex from;
    TileIndex to;
    uint agent_height;

    uint allowed_fall = 2;
    uint allowed_climb = 1;
};
