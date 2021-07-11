#pragma once

#include "Pointers.h"

class Animation;

struct AnimationSlot
{
    Shared<Animation> animation;
    bool reverse;
};
