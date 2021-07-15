#pragma once

#include "Engine/Name.h"

#define META_TAG(name) inline static const Name name = #name;

namespace MetaTags
{
    META_TAG(AIR);
    META_TAG(SOLID);
    META_TAG(COMPLEX);
    
    META_TAG(DIRT);
    META_TAG(STONE);
    META_TAG(IRON);

    META_TAG(PLANT);
    META_TAG(GRASS);
    META_TAG(WOOD);
    META_TAG(FLOWER);
    META_TAG(STEM);
}
