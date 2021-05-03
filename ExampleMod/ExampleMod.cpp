#include "ExampleMod.h"

#include <memory>


#include "Engine/EventBus.h"
#include "HexaGame/DemoMeshEntity.h"

void ExampleMod::on_init(EventBus* event_bus)
{
    event_bus->world_opened.bind(this, &ExampleMod::on_world_opened);
}

void ExampleMod::on_world_opened(const Weak<World>& world)
{
    if (auto world_ptr = world.lock())
    {
        auto entity = MakeShared<DemoMeshEntity>();
        world_ptr->spawn_entity(entity, Vector3(0.0f, 0.0f, 0.0f));
    }
}
