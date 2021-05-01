#pragma once
#include "BasicTypes.h"
#include "Delegate.h"
#include "World.h"

class EventBus
{
public:
    Delegate<const Weak<World>&> world_opened;
    Delegate<const Weak<World>&> world_closed;
};
