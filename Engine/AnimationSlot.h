#pragma once
#include "BasicTypes.h"

class Animation;

struct AnimationSlot
{
    Shared<Animation> animation;
    bool reverse;
};
