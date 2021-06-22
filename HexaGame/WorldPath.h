#pragma once

#include "TileIndex.h"
#include "TileSide.h"

class WorldPath
{
public:
    struct Segment
    {
        TileIndex from;
        TileIndex to;
    };

    List<Segment> segments;
};
