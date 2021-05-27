#pragma once

#include "BasicTypes.h"
#include "Delegate.h"
#include "framework.h"

class World;

EXTERN class EXPORT EventBus
{
public:
    Delegate<const Weak<World>&> world_opened;
    Delegate<const Weak<World>&> world_closed;
};
