#pragma once

#include "BasicTypes.h"
#include "Delegate.h"
#include "framework.h"

class World;

class EXPORT EventBus
{
public:
    Delegate<const Shared<World>&> world_opened;
    Delegate<const Shared<World>&> world_closed;
};
