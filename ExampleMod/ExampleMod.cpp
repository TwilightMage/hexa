#include "ExampleMod.h"

#include <memory>


#include "Engine/EventBus.h"

void ExampleMod::on_loaded(const Shared<EventBus>& event_bus)
{
    event_bus->world_opened.bind(this, &ExampleMod::on_world_opened);
}

void ExampleMod::on_world_opened(const Shared<World>& world)
{
    
}
