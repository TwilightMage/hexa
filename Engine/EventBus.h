#pragma once

#include "BasicTypes.h"
#include "Delegate.h"

class World;

class EventBus
{
public:
    Delegate<const Weak<World>&> world_opened;
    Delegate<const Weak<World>&> world_closed;
};
