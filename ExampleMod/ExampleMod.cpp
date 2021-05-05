#include "ExampleMod.h"

#include <memory>


#include "Engine/EventBus.h"

void ExampleMod::on_loaded(EventBus* event_bus)
{
    event_bus->world_opened.bind(this, &ExampleMod::on_world_opened);
}

void ExampleMod::on_world_opened(const Weak<World>& world)
{
    if (auto world_ptr = world.lock())
    {
        
    }
}
