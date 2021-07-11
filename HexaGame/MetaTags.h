#pragma once

#include "Engine/String.h"

#define META_TAG(name) inline static const String name = #name;

namespace MetaTags
{
    META_TAG(AIR);
    META_TAG(SOLID);
    META_TAG(COMPLEX);
    
    META_TAG(DIRT);
    META_TAG(STONE);

    META_TAG(PLANT);
    META_TAG(GRASS);
    META_TAG(WOOD);
    META_TAG(FLOWER);
}
