#include "World.h"

#include "ITickable.h"

void World::setup_spawn(WorldObject* object)
{
    object->world = this;
    objects.Add(object);
    object->start();
}

void World::start()
{
    on_start();
}

void World::tick(float delta_time)
{
    on_tick();
    for (auto object : objects)
    {
        if (auto tickable = Cast<ITickable>(object))
        {
            tickable->tick(delta_time);
        }
    }
}

void World::on_start()
{
}

void World::on_tick()
{
}
